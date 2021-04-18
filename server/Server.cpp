#include "Server.h"
#include "Login.h"
#include "../common/Pipe.h"
#include "../common/MoveSystem.h"
#include "../common/Position.h"
#include "../common/Velocity.h"
#include "../common/Voxel.h"
#include "../common/VoxelSystem.h"
#include "../common/Math.h"
#include "../common/UUID.h"
#include <Eigen/Eigen>
#include <ZSerializer.hpp>
#include <thread>
#include <chrono>

using namespace std::placeholders;
using namespace ct;

namespace
{
	struct ConnectionInfo
	{
		uint64_t connectionID;
		std::weak_ptr<ct::NetAgent<>> agent;
	};
} // namespace

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

			if (auto voxel = e.Get<Voxel>())
			{
				zs::Write(components, "voxel");
				zs::Write(components, *voxel);
			}

			zs::Write(out, components.String());
		}
	};
}

namespace ct
{
	void Server::Run()
	{
		auto endpoint = asio::ip::tcp::endpoint{ asio::ip::tcp::v4(),33773 };
		ct::Acceptor acceptor{ io_,endpoint,std::bind(&Server::OnConnection, this, _1, _2) };

		voxel_system::GenerateVoxels(entities_);

		auto interval = std::chrono::milliseconds{ 33 };
		auto shouldTick = std::chrono::steady_clock::now();
		while (true)
		{
			io_.poll();
			move_system::Process(entities_, interval.count() / 1000.f);
			voxel_system::Process(entities_, interval.count() / 1000.f);

			zs::StringWriter worldOut;
			entities_.ForEach([&](EntityHandle e) {zs::Write(worldOut, e);});
			auto world = worldOut.String();
			for (auto& [_, agent] : agents_)
				agent->Send("world", world);

			shouldTick += interval;
			io_.run_until(shouldTick);
		}
	}

	void Server::OnConnection(const std::error_code& error, asio::ip::tcp::socket&& socket)
	{
		if (error)
			return;
		auto pipe = std::make_shared<Pipe<Socket>>(std::move(socket));
		auto login = std::make_shared<Login<Pipe<Socket>>>(pipe, ++connectionID_, io_, std::chrono::seconds{ 3 });
		login->OnSuccess(
			[login, pipe, this](uint64_t id) {
				OnLoginSuccess(pipe, id);
			});
		login->OnError(
			[login]() {
			});
	}

	void Server::OnLoginSuccess(std::shared_ptr<Pipe<>> pipe, uint64_t connectionID)
	{
		auto agent = std::make_shared<NetAgent<>>(pipe);
		agents_[connectionID] = agent;

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
			agents_.erase(connectionID);
			});
	}
} // namespace ct