#pragma once
#include <asio.hpp>
#include "../../common/source/Acceptor.h"
#include "../../common/source/Socket.h"

namespace ct
{
	class Server
	{
	public:
		Server(asio::io_context& io, const asio::ip::tcp::endpoint& endpoint);

	private:
		void OnConnection(const std::error_code&, asio::ip::tcp::socket&&);

	private:
		asio::io_context& io_;
		ct::Acceptor acceptor_;
		uint64_t connectionID_ = 0;
	};
}
