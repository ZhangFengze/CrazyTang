#pragma once
#include <asio.hpp>

namespace ct
{
	struct Connection
	{
		Connection(asio::ip::tcp::socket&& s) :socket(std::move(s)) {}
		asio::ip::tcp::socket socket;
		asio::streambuf buffer{1024};
	};

	struct ConnectionEvent
	{
		std::string from;
	};

    struct DataEvent
    {
		std::string from;
		std::string data;
    };

	struct DisconnectionEvent
	{
		std::string from;
	};


	class Net
	{
	public:
		Net(asio::io_context&, const asio::ip::tcp::endpoint&);

		void Connect(const asio::ip::tcp::endpoint&);
		void Broadcast(const std::string&);

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
