#include "Net.h"

using asio::ip::tcp;

namespace ct
{
	Net::Net(asio::io_context& io)
		:acceptor_(io, tcp::endpoint(tcp::v4(), 8888))
	{
		StartAccept();
	}

	void Net::StartAccept()
	{
		acceptor_.async_accept(
			[this](const asio::error_code& error, tcp::socket&& socket)
		{
			if (!error)
				HandleAccept(std::move(socket));
			StartAccept();
		});
	}

	void Net::HandleAccept(tcp::socket&& socket)
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

			HandleMessage(connection);
			StartRead(connection);
		});
	}

	void Net::HandleMessage(Connection& connection)
	{
		asio::write(connection.socket, asio::buffer(connection.buffer));
	}

	void Net::RemoveConnection(Connection& connection)
	{
		connections_.erase(
			std::remove_if(connections_.begin(), connections_.end(),
				[&connection](std::shared_ptr<Connection> p) {return p.get() == &connection; }),
			connections_.end());
	}
}