#pragma once
#include "ConnectionInfo.h"
#include "../common/Entity.h"
#include "../common/Net.h"
#include "../common/Voxel.h"
#include "../common/fps.h"
#include <asio.hpp>
#include <cstdint>
#include <memory>
#include <unordered_map>

namespace ct
{
	class Server
	{
	public:
		void Run();

	private:
		asio::awaitable<void> Listen();
		asio::awaitable<void> OnConnection(asio::ip::tcp::socket);
		void OnLoginSuccess(std::shared_ptr<NetAgent> agent, uint64_t connectionID);

		asio::awaitable<void> LogFps();

	private:
		asio::io_context io_;
		uint64_t connectionID_ = 0;
		std::unordered_map<uint64_t, Connection> connections_;
		EntityContainer entities_;
		voxel::Container voxels_;
		FPS fps_;
	};
}
