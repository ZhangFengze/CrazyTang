#include "Net.h"

using asio::ip::tcp;

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

	void Net::Broadcast(std::vector<uint8_t>&& data)
	{
		auto buffer = std::make_shared<std::vector<uint8_t>>(std::move(data));
		for (auto& connection : connections_)
		{
			asio::async_write(connection->socket, asio::buffer(*buffer),
				[this,connection,buffer](const asio::error_code& error, size_t size)
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
		auto connection = std::make_shared<Connection>(Connection{ std::move(socket) });
		connections_.push_back(connection);
		StartRead(*connections_.back());
	}

	void Net::StartRead(Connection& connection)
	{
		connection.socket.async_read_some(asio::buffer(connection.buffer),
			[&connection,this](const asio::error_code& error, size_t size)
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
	}

	void Net::RemoveConnection(Connection& connection)
	{
		connections_.erase(
			std::remove_if(connections_.begin(), connections_.end(),
				[&connection](std::shared_ptr<Connection> p) {return p.get() == &connection; }),
			connections_.end());
	}
}