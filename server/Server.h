#pragma once
#include "../common/NetAgent.h"
#include "../common/Acceptor.h"
#include "../common/Entity.h"
#include "../common/Pipe.h"
#include "../common/Voxel.h"
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
		void OnConnection(const std::error_code&, asio::ip::tcp::socket&&);
		void OnLoginSuccess(std::shared_ptr<Pipe<>> pipe, uint64_t connectionID);

	private:
		asio::io_context io_;
		uint64_t connectionID_ = 0;
		std::unordered_map<uint64_t, std::shared_ptr<NetAgent<>>> agents_;
		EntityContainer entities_;
		voxel::Container voxels_;
	};
}
