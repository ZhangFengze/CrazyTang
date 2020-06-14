#pragma once
#include <asio.hpp>

namespace ct
{
	struct Connection
	{
		asio::ip::tcp::socket socket;
		std::array<char, 1024> buffer;
	};

	class Net
	{
	public:
		Net(asio::io_context&, const asio::ip::tcp::endpoint&);

		void Connect(const asio::ip::tcp::endpoint&);
		void Broadcast(std::vector<uint8_t>&&);

	private:
		void StartAccept();
		void AddConnection(asio::ip::tcp::socket&&);

		void StartRead(Connection&);
		void HandleMessage(Connection&, size_t);
		void RemoveConnection(Connection&);

	private:
		asio::io_context& io_;
		asio::ip::tcp::acceptor acceptor_;
		std::vector<std::shared_ptr<Connection>> connections_;
	};
}
