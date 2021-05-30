#include "Server.h"
#include "Login.h"
#include "ConnectionInfo.h"
#include "VoxelWatcher.h"
#include "../common/Net.h"
#include "../common/MoveSystem.h"
#include "../common/Position.h"
#include "../common/Velocity.h"
#include "../common/Name.h"
#include "../common/Voxel.h"
#include "../common/Math.h"
#include "../common/UUID.h"
#include <Eigen/Eigen>
#include <ZSerializer.hpp>
#include <format>
#include <thread>
#include <chrono>
#include <iostream>

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

			if (auto name = e.Get<xy::Name>())
			{
				zs::Write(components, "name");
				zs::Write(components, *name);
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
		if (!co_await ServerLogin(socket, id, std::chrono::seconds{ 3 }))
			co_return;
		auto agent = std::make_shared<NetAgent>(std::move(socket));
		OnLoginSuccess(agent, id);
		co_spawn(co_await asio::this_coro::executor, [agent]() -> asio::awaitable<void>
			{
				co_await agent->ReadRoutine();
			}, asio::detached);

		co_spawn(co_await asio::this_coro::executor, [agent]() -> asio::awaitable<void>
			{
				co_await agent->WriteRoutine();
			}, asio::detached);
	}

	asio::awaitable<void> Server::LogFps()
	{
		asio::system_timer timer(co_await asio::this_coro::executor);
		while (true)
		{
			timer.expires_after(std::chrono::seconds{ 1 });
			co_await timer.async_wait(asio::use_awaitable);
#ifdef _WIN32
			std::cout << std::format("{} fps: {}\n", std::chrono::system_clock::now(), fps_.get());
#endif
		}
	}

	void Server::Run()
	{
		asio::co_spawn(io_, Listen(), asio::detached);
		asio::co_spawn(io_, LogFps(), asio::detached);

		voxel::GenerateVoxels(voxels_);

		auto interval = std::chrono::milliseconds{ 33 };
		auto shouldTick = std::chrono::steady_clock::now();
		while (true)
		{
			io_.poll();
			move_system::Process(entities_, interval.count() / 1000.f);
			// voxel::Process(voxels_, interval.count() / 1000.f);

			zs::StringWriter worldOut;
			entities_.ForEach([&](EntityHandle e) {zs::Write(worldOut, e);});
			auto world = worldOut.String();

			for (auto& [_, connection] : connections_)
			{
				connection.agent->Send("entities", world);
			}

			// voxel_watcher::Process(entities_, voxels_, interval.count() / 1000.f);

			fps_.fire();
			shouldTick += interval;
			io_.run_until(shouldTick);
		}
	}

	void Server::OnLoginSuccess(std::shared_ptr<NetAgent> agent, uint64_t connectionID)
	{
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

		auto name = e.Add<xy::Name>();
		name->data = "zjx";

		auto info = e.Add<ConnectionInfo>();
		info->connectionID = connectionID;
		info->agent = agent;

		agent->Listen("set position",
			[this, e](std::string&& data) mutable {
				zs::StringReader in{ std::move(data) };
				auto pos = zs::Read<Eigen::Vector3f>(in);
				if (std::holds_alternative<zs::Error>(pos))
					return;
				e.Get<Position>()->data = std::get<0>(pos);
			});

		agent->Listen("set velocity",
			[this, e](std::string&& data) mutable {
				zs::StringReader in{ std::move(data) };
				auto vel = zs::Read<Eigen::Vector3f>(in);
				if (std::holds_alternative<zs::Error>(vel))
					return;
				e.Get<Velocity>()->data = std::get<0>(vel);
			});

		agent->Listen("set name",
			[this, e](std::string&& data) mutable {
				zs::StringReader in{ std::move(data) };
				auto name = zs::Read<std::string>(in);
				if (std::holds_alternative<zs::Error>(name))
					return;
				e.Get<xy::Name>()->data = std::get<0>(name);
			});

		agent->OnError([e, connectionID, this]() mutable {
			if (e.Valid())
				e.Destroy();
			connections_.erase(connectionID);
			});

		voxel_watcher::Sync(e, voxels_);
	}
} // namespace ct