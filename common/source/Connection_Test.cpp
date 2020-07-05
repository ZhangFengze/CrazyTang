#ifdef CT_TEST
#include <catch.hpp>
#include "Connection.h"

using asio::ip::tcp;

namespace
{
	tcp::endpoint AvailableLocalEndpoint()
	{
		return tcp::endpoint{ asio::ip::make_address("127.0.0.1"),53453 };
	}
}

TEST_CASE("construction", "[Connection]")
{
	asio::io_context io;
	ct::Connection connection{ tcp::socket{io} };
}

TEST_CASE("async read, async write", "[Connection]")
{
	asio::io_context io;
	auto endpoint = AvailableLocalEndpoint();

	std::shared_ptr<ct::Connection> connection;

	tcp::acceptor acceptor{ io,endpoint };
	acceptor.async_accept([&connection](asio::error_code error, tcp::socket&& socket)
	{
		REQUIRE(!error);
		connection = std::make_shared<ct::Connection>(std::move(socket));
		connection->AsyncReadPacket([](std::error_code error, const char* data, size_t size)
		{
			REQUIRE(!error);
			REQUIRE(size == 6);
			REQUIRE(strcmp(data, "hello") == 0);
		});
	});

	tcp::socket socket{ io };
	socket.async_connect(endpoint, [&socket](asio::error_code error)
	{
		REQUIRE(!error);
		ct::Connection c{ std::move(socket) };
		c.AsyncWritePacket("hello", 6,
			[](std::error_code error) 
		{
			REQUIRE(!error);
		});
	});

	io.run();
}

TEST_CASE("multiple read write", "[Connection]")
{
	asio::io_context io;
	auto endpoint = AvailableLocalEndpoint();

	std::shared_ptr<ct::Connection> connection;

	tcp::acceptor acceptor{ io,endpoint };
	acceptor.async_accept([&connection](asio::error_code error, tcp::socket&& socket)
	{
		REQUIRE(!error);
		connection = std::make_shared<ct::Connection>(std::move(socket));
		connection->AsyncReadPacket([connection](std::error_code error, const char* data, size_t size)
		{
			REQUIRE(!error);
			REQUIRE(size == 17);
			REQUIRE(strcmp(data, "first hello here") == 0);

			connection->AsyncReadPacket([](std::error_code error, const char* data, size_t size)
			{
				REQUIRE(!error);
				REQUIRE(size == 14);
				REQUIRE(strcmp(data, "that's second") == 0);
			});
		});
	});

	tcp::socket socket{ io };
	socket.async_connect(endpoint, [&socket](asio::error_code error)
	{
		REQUIRE(!error);
		auto c = std::make_shared<ct::Connection>(std::move(socket));
		c->AsyncWritePacket("first hello here", 17,
			[c](std::error_code error)
		{
			REQUIRE(!error);
			
			c->AsyncWritePacket("that's second", 14,
				[c](std::error_code error)
			{
				REQUIRE(!error);
			});
		});
	});

	io.poll();
}
#endif
