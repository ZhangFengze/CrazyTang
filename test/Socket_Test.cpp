#include <catch.hpp>
#include "Socket.h"

using namespace std::literals;
using asio::ip::tcp;

namespace
{
	tcp::endpoint AvailableLocalEndpoint()
	{
		return tcp::endpoint{ asio::ip::make_address("127.0.0.1"),53453 };
	}
	const ct::Packet packet0 = "hello"sv;
	const ct::Packet packet1 = "first hello"sv;
	const ct::Packet packet2 = "second"sv;
}

TEST_CASE("construction", "[Socket]")
{
	asio::io_context io;
	ct::Socket socket{ tcp::socket{io} };
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
		socket->AsyncReadPacket([](std::error_code error, ct::Packet&& packet)
		{
			REQUIRE(!error);
			REQUIRE(packet == packet0);
		});
	});

	tcp::socket lowLevelSocket{ io };
	lowLevelSocket.async_connect(endpoint, [&lowLevelSocket](asio::error_code error)
	{
		REQUIRE(!error);
		ct::Socket c{ std::move(lowLevelSocket) };
		c.AsyncWritePacket(packet0,
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
		socket->AsyncReadPacket([socket](std::error_code error, ct::Packet&& packet)
		{
			REQUIRE(!error);
			REQUIRE(packet == packet1);

			socket->AsyncReadPacket([](std::error_code error, ct::Packet&& packet)
			{
				REQUIRE(!error);
				REQUIRE(packet == packet2);
			});
		});
	});

	tcp::socket lowLevelSocket{ io };
	lowLevelSocket.async_connect(endpoint, [&lowLevelSocket](asio::error_code error)
	{
		REQUIRE(!error);
		auto c = std::make_shared<ct::Socket>(std::move(lowLevelSocket));
		c->AsyncWritePacket(packet1, 
			[c](std::error_code error)
		{
			REQUIRE(!error);
			
			c->AsyncWritePacket(packet2,
				[c](std::error_code error)
			{
				REQUIRE(!error);
			});
		});
	});

	io.poll();
}