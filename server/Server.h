#pragma once
#include "../common/NetAgent.h"
#include "../common/Acceptor.h"
#include "../common/Entity.h"
#include "../common/Pipe.h"
#include <asio.hpp>
#include <memory>
#include <unordered_map>

namespace ct
{
	class Server
	{
	public:
		Server(asio::io_context& io, const asio::ip::tcp::endpoint& endpoint);

	private:
		void OnConnection(const std::error_code&, asio::ip::tcp::socket&&);
		void OnLoginSuccess(std::shared_ptr<Pipe<>> pipe, uint64_t connectionID);

	private:
		asio::io_context& io_;
		ct::Acceptor acceptor_;
		uint64_t connectionID_ = 0;
		std::unordered_map<uint64_t, std::shared_ptr<NetAgent<>>> agents_;
		EntityContainer entities_;
	};
}
