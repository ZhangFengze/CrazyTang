#include "Net.h"
#include "Game.h"
#include "Replication.h"

using asio::ip::tcp;

namespace
{
	std::string ToString(const tcp::endpoint& endpoint)
	{
		std::ostringstream os;
		os << endpoint;
		return os.str();
	}
}

namespace ct
{
	Net::Net(asio::io_context& io, const asio::ip::tcp::endpoint& endpoint)
		:io_(io),
		acceptor_(io, endpoint, false)
	{
		StartAccept();
	}

	void Net::Connect(const asio::ip::tcp::endpoint& endpoint)
	{
		auto socket = std::make_shared<tcp::socket>(io_);
		socket->async_connect(endpoint,
			[this,socket](const asio::error_code& error)
		{
			if (!error)
				AddConnection(std::move(*socket));
		});
	}

	void Net::Broadcast(const std::string& raw)
	{
		assert(raw.find('\n') == std::string::npos);
		auto data = std::make_shared<std::string>(raw + "\n");
		for (auto& connection : connections_)
		{
			asio::async_write(connection->socket, asio::buffer(*data),
				[this, connection, data](const asio::error_code& error, size_t size)
			{
				if (error)
					RemoveConnection(*connection);
			});
		}
	}

	void Net::StartAccept()
	{
		acceptor_.async_accept(
			[this](const asio::error_code& error, tcp::socket&& socket)
		{
			if (!error)
				AddConnection(std::move(socket));
			StartAccept();
		});
	}

	void Net::AddConnection(tcp::socket&& socket)
	{
		auto connection = std::make_shared<Connection>(std::move(socket));
		connections_.push_back(connection);
		StartRead(*connections_.back());

		game->events.emit(ConnectionEvent{ ToString(connection->socket.remote_endpoint()) });
	}

	void Net::StartRead(Connection& connection)
	{
		asio::async_read_until(connection.socket, connection.buffer, '\n', 
			[&connection, this](const asio::error_code& error, size_t size)
		{
			if (error)
			{
				RemoveConnection(connection);
				return;
			}

			HandleMessage(connection, size);
			StartRead(connection);
		});
	}

	void Net::HandleMessage(Connection& connection, size_t size)
	{
		std::istream in(&connection.buffer);

		DataEvent event;

		event.from = ToString(connection.socket.remote_endpoint());

		event.data.resize(size - 1);
		in.read(event.data.data(), size - 1);

		char c;
		in.read(&c, 1);

		game->events.emit<DataEvent>(event);
	}

	void Net::RemoveConnection(Connection& connection)
	{
		auto remote = ToString(connection.socket.remote_endpoint());

		connections_.erase(
			std::remove_if(connections_.begin(), connections_.end(),
				[&connection](std::shared_ptr<Connection> p) {return p.get() == &connection; }),
			connections_.end());

		game->events.emit(DisconnectionEvent{ remote });
	}
}