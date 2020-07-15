#pragma once
#include <asio.hpp>
#include "../../common/source/Acceptor.h"
#include "../../common/source/Socket.h"

namespace ct
{
	class BroadcastServer
	{
	public:
		BroadcastServer(asio::io_context& io, const asio::ip::tcp::endpoint& endpoint);

	private:
		void Broadcast(std::shared_ptr<Socket> socket);
		void Remove(std::shared_ptr<Socket> socket);

	private:
		ct::Acceptor acceptor_;
		std::vector<std::shared_ptr<ct::Socket>> sockets_;
	};
}
