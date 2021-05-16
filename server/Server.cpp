#include "Server.h"
#include "Login.h"
#include "ConnectionInfo.h"
#include "VoxelWatcher.h"
#include "../common/Pipe.h"
#include "../common/MoveSystem.h"
#include "../common/Position.h"
#include "../common/Velocity.h"
#include "../common/Voxel.h"
#include "../common/Math.h"
#include "../common/UUID.h"
#include <Eigen/Eigen>
#include <ZSerializer.hpp>
#include <thread>
#include <chrono>

using namespace std::placeholders;
using namespace ct;

namespace zs
{
	template<>
	struct Trait<ct::EntityHandle>
	{
		template<typename Out>
		static void Write(Out& out, const ct::EntityHandle& e)
		{
			assert(e.Has<UUID>());
			zs::Write(out, *e.Get<UUID>());

			zs::StringWriter components;
			if (auto connection = e.Get<ConnectionInfo>())
			{
				zs::Write(components, "connection");
				zs::Write(components, connection->connectionID);
			}

			if (auto position = e.Get<Position>())
			{
				zs::Write(components, "position");
				zs::Write(components, *position);
			}

			if (auto velocity = e.Get<Velocity>())
			{
				zs::Write(components, "velocity");
				zs::Write(components, *velocity);
			}

			zs::Write(out, components.String());
		}
	};
}

namespace ct
{
	asio::awaitable<void> Server::Listen()
	{
		auto endpoint = asio::ip::tcp::endpoint{ asio::ip::tcp::v4(),33773 };
		asio::ip::tcp::acceptor acceptor{ io_, endpoint };
		for (;;)
		{
			auto socket = co_await acceptor.async_accept(asio::use_awaitable);
			co_spawn(io_, OnConnection(std::move(socket)), asio::detached);
		}
	}

	asio::awaitable<void> Server::OnConnection(asio::ip::tcp::socket socket)
	{
		auto id = ++connectionID_;
		if (co_await ServerLogin(socket, id, std::chrono::seconds{ 3 }))
		{
			auto pipe = std::make_shared<Pipe<Socket>>(std::move(socket));
			OnLoginSuccess(pipe, id);
		}
	}

	void Server::Run()
	{
		asio::co_spawn(io_, Listen(), asio::detached);

		voxel::GenerateVoxels(voxels_);

		auto interval = std::chrono::milliseconds{ 33 };
		auto shouldTick = std::chrono::steady_clock::now();
		while (true)
		{
			io_.poll();
			move_system::Process(entities_, interval.count() / 1000.f);
			voxel::Process(voxels_, interval.count() / 1000.f);

			zs::StringWriter worldOut;
			entities_.ForEach([&](EntityHandle e) {zs::Write(worldOut, e);});
			auto world = worldOut.String();

			for (auto& [_, connection] : connections_)
			{
				connection.agent->Send("entities", world);
			}

			voxel_watcher::Process(entities_, voxels_, interval.count() / 1000.f);

			shouldTick += interval;
			io_.run_until(shouldTick);
		}
	}

	void Server::OnLoginSuccess(std::shared_ptr<Pipe<>> pipe, uint64_t connectionID)
	{
		auto agent = std::make_shared<NetAgent<>>(pipe);

		Connection connection;
		connection.connectionID = connectionID;
		connection.agent = agent;
		connections_[connectionID] = connection;

		auto e = entities_.Create();

		auto uuid = e.Add<UUID>();
		uuid->id = GenerateUUID();

		auto position = e.Add<Position>();
		position->data.x() = 0;
		position->data.y() = 0;
		position->data.z() = 0;

		auto velocity = e.Add<Velocity>();
		velocity->data.x() = 0;
		velocity->data.y() = 0;
		velocity->data.z() = 0;

		auto info = e.Add<ConnectionInfo>();
		info->connectionID = connectionID;
		info->agent = agent;

		agent->Listen("set position",
			[this, agent, e](std::string&& data) mutable {
				zs::StringReader in{ std::move(data) };
				auto pos = zs::Read<Eigen::Vector3f>(in);
				if (std::holds_alternative<zs::Error>(pos))
					return;
				e.Get<Position>()->data = std::get<0>(pos);
			});

		agent->Listen("set velocity",
			[this, agent, e](std::string&& data) mutable {
				zs::StringReader in{ std::move(data) };
				auto vel = zs::Read<Eigen::Vector3f>(in);
				if (std::holds_alternative<zs::Error>(vel))
					return;
				e.Get<Velocity>()->data = std::get<0>(vel);
			});

		agent->OnError([e, connectionID, this]() mutable {
			if (e.Valid())
				e.Destroy();
			connections_.erase(connectionID);
			});
	}
} // namespace ct