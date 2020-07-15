#include "Server.h"
namespace ct
{
	BroadcastServer::BroadcastServer(asio::io_context& io, const asio::ip::tcp::endpoint& endpoint)
		:acceptor_(io, endpoint,
			[this](asio::error_code error, asio::ip::tcp::socket&& lowLevelSocket)
		{
			if (!error)
			{
				auto socket = std::make_shared<Connection>(std::move(lowLevelSocket));
				sockets_.push_back(socket);
				Broadcast(socket);
			}
		})
	{
	}

	void BroadcastServer::Broadcast(std::shared_ptr<Connection> socket)
	{
		socket->AsyncReadPacket(
			[this, socket](std::error_code error, const char* data, size_t size)
		{
			if (error)
				return Remove(socket);

			for (auto peer : sockets_)
			{
				if (peer == socket)
					continue;
				peer->AsyncWritePacket(data, size,
					[this, peer](std::error_code error)
				{
					if (error)
						Remove(peer);
				});
			}
			Broadcast(socket);
		});
	}

	void BroadcastServer::Remove(std::shared_ptr<Connection> socket)
	{
		sockets_.erase(
			std::find(sockets_.begin(), sockets_.end(), socket));
	}
}