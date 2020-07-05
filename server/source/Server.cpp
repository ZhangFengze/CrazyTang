#include "Server.h"
namespace ct
{
	BroadcastServer::BroadcastServer(asio::io_context& io, const asio::ip::tcp::endpoint& endpoint)
		:acceptor_(io, endpoint,
			[this](asio::error_code error, asio::ip::tcp::socket&& socket)
		{
			if (!error)
			{
				auto connection = std::make_shared<Connection>(std::move(socket));
				connections_.push_back(connection);
				Broadcast(connection);
			}
		})
	{
	}

	void BroadcastServer::Broadcast(std::shared_ptr<Connection> connection)
	{
		connection->AsyncReadPacket(
			[this, connection](std::error_code error, const char* data, size_t size)
		{
			if (error)
				return Remove(connection);

			for (auto peer : connections_)
			{
				if (peer == connection)
					continue;
				peer->AsyncWritePacket(data, size,
					[this, peer](std::error_code error)
				{
					if (error)
						Remove(peer);
				});
			}
			Broadcast(connection);
		});
	}

	void BroadcastServer::Remove(std::shared_ptr<Connection> connection)
	{
		connections_.erase(
			std::find(connections_.begin(), connections_.end(), connection));
	}
}