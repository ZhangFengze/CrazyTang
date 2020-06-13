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
		Net(asio::io_context&);

	private:
		void StartAccept();
		void HandleAccept(asio::ip::tcp::socket&&);

		void StartRead(Connection&);
		void HandleMessage(Connection&);
		void RemoveConnection(Connection&);

	private:
		asio::ip::tcp::acceptor acceptor_;
		std::vector<std::shared_ptr<Connection>> connections_;
	};
}
