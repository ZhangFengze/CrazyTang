#ifdef CT_TEST
#include <catch.hpp>
#include "Socket.h"

using asio::ip::tcp;

namespace
{
	tcp::endpoint AvailableLocalEndpoint()
	{
		return tcp::endpoint{ asio::ip::make_address("127.0.0.1"),53453 };
	}
}

TEST_CASE("construction", "[Socket]")
{
	asio::io_context io;
	ct::Socket connection{ tcp::socket{io} };
}

TEST_CASE("async read, async write", "[Socket]")
{
	asio::io_context io;
	auto endpoint = AvailableLocalEndpoint();

	std::shared_ptr<ct::Socket> socket;

	tcp::acceptor acceptor{ io,endpoint };
	acceptor.async_accept([&socket](asio::error_code error, tcp::socket&& lowLevelSocket)
	{
		REQUIRE(!error);
		socket = std::make_shared<ct::Socket>(std::move(lowLevelSocket));
		socket->AsyncReadPacket([](std::error_code error, const char* data, size_t size)
		{
			REQUIRE(!error);
			REQUIRE(size == 6);
			REQUIRE(strcmp(data, "hello") == 0);
		});
	});

	tcp::socket lowLevelSocket{ io };
	lowLevelSocket.async_connect(endpoint, [&lowLevelSocket](asio::error_code error)
	{
		REQUIRE(!error);
		ct::Socket c{ std::move(lowLevelSocket) };
		c.AsyncWritePacket("hello", 6,
			[](std::error_code error) 
		{
			REQUIRE(!error);
		});
	});

	io.run();
}

TEST_CASE("multiple read write", "[Socket]")
{
	asio::io_context io;
	auto endpoint = AvailableLocalEndpoint();

	std::shared_ptr<ct::Socket> socket;

	tcp::acceptor acceptor{ io,endpoint };
	acceptor.async_accept([&socket](asio::error_code error, tcp::socket&& lowLevelSocket)
	{
		REQUIRE(!error);
		socket = std::make_shared<ct::Socket>(std::move(lowLevelSocket));
		socket->AsyncReadPacket([socket](std::error_code error, const char* data, size_t size)
		{
			REQUIRE(!error);
			REQUIRE(size == 17);
			REQUIRE(strcmp(data, "first hello here") == 0);

			socket->AsyncReadPacket([](std::error_code error, const char* data, size_t size)
			{
				REQUIRE(!error);
				REQUIRE(size == 14);
				REQUIRE(strcmp(data, "that's second") == 0);
			});
		});
	});

	tcp::socket lowLevelSocket{ io };
	lowLevelSocket.async_connect(endpoint, [&lowLevelSocket](asio::error_code error)
	{
		REQUIRE(!error);
		auto c = std::make_shared<ct::Socket>(std::move(lowLevelSocket));
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
