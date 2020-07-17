#ifdef CT_TEST
#include <catch.hpp>
#include "Socket.h"
#include "Pipe.h"

namespace
{
	const std::string packet0{ "hello world" };
	const std::string packet1{ "no bugs" };
	const std::string packet2{ "long story" };
	const std::string packet3{ "stop!" };
}

TEST_CASE("pipe construction")
{
	ct::Pipe<ct::StubSocket> pipe{ ct::StubSocket{} };
}

TEST_CASE("pipe write packet")
{
	asio::io_context io;
	ct::Pipe<ct::MockSocket> pipe{ ct::MockSocket{io} };

	pipe.SendPacket(packet0.data(), packet0.size());
	pipe.SendPacket(packet1.data(), packet1.size());

	io.poll();

	REQUIRE(pipe.socket_.writtenPackets.size() == 2);

	REQUIRE(packet0 == pipe.socket_.writtenPackets[0]);
	REQUIRE(packet1 == pipe.socket_.writtenPackets[1]);
}

TEST_CASE("pipe read packet")
{
	asio::io_context io;
	ct::Pipe<ct::MockSocket> pipe{ ct::MockSocket{io} };

	std::vector<std::string> packets;
	pipe.OnPacket([&packets](const char* data, size_t size)
	{
		packets.push_back(std::string{ data,size });
	});

	pipe.socket_.PacketArrive({}, packet0.data(), packet0.size());
	pipe.socket_.PacketArrive({}, packet1.data(), packet1.size());

	io.poll();

	REQUIRE(packets.size() == 2);

	REQUIRE(packet0 == packets[0]);
	REQUIRE(packet1 == packets[1]);
}

TEST_CASE("pipe broken")
{
	asio::io_context io;
	ct::Pipe<ct::MockSocket> pipe{ ct::MockSocket{io} };
	REQUIRE(!pipe.IsBroken());

	bool broken = false;
	pipe.OnPacket([](const char*, size_t) {});
	pipe.OnBroken([&broken]()
	{
		REQUIRE(!broken);
		broken = true;
	});

	pipe.socket_.PacketArrive({}, packet0.data(), packet0.size());
	pipe.socket_.PacketArrive({}, packet1.data(), packet1.size());
	pipe.socket_.PacketArrive(asio::error::eof, nullptr, 0);

	io.poll();

	REQUIRE(broken);
	REQUIRE(pipe.IsBroken());
}

TEST_CASE("pipe should stop working after broken")
{
	asio::io_context io;
	ct::Pipe<ct::MockSocket> pipe{ ct::MockSocket{io} };

	bool broken = false;
	std::vector<std::string> packets;
	pipe.OnPacket([&packets,&broken](const char* data, size_t size)
	{
		REQUIRE(!broken);
		packets.push_back(std::string{ data,size });
	});
	pipe.OnBroken([&broken]()
	{
		REQUIRE(!broken);
		broken = true;
	});

	pipe.socket_.PacketArrive({}, packet0.data(), packet0.size());
	pipe.socket_.PacketArrive({}, packet1.data(), packet1.size());
	pipe.socket_.PacketArrive(asio::error::eof, nullptr, 0);
	pipe.socket_.PacketArrive({}, packet2.data(), packet2.size());

	pipe.SendPacket(packet3.data(), packet3.size());

	io.poll();

	REQUIRE(packets.size() == 2);
	REQUIRE(broken);
	REQUIRE(pipe.socket_.writtenPackets.empty());
}

TEST_CASE("pipe change packet handler")
{
	asio::io_context io;
	ct::Pipe<ct::MockSocket> pipe{ ct::MockSocket{io} };

	std::vector<std::string> packets0;
	pipe.OnPacket([&packets0](const char* data, size_t size)
	{
		packets0.push_back(std::string{ data,size });
	});

	pipe.socket_.PacketArrive({}, packet0.data(), packet0.size());
	pipe.socket_.PacketArrive({}, packet1.data(), packet1.size());

	io.poll();

	std::vector<std::string> packets1;
	pipe.OnPacket([&packets1](const char* data, size_t size)
	{
		packets1.push_back(std::string{ data,size });
	});
	
	pipe.socket_.PacketArrive({}, packet2.data(), packet2.size());
	pipe.socket_.PacketArrive({}, packet3.data(), packet3.size());

	io.poll();

	REQUIRE(packets0.size() == 2);
	REQUIRE(packet0 == packets0[0]);
	REQUIRE(packet1 == packets0[1]);

	REQUIRE(packets1.size() == 2);
	REQUIRE(packet2 == packets1[0]);
	REQUIRE(packet3 == packets1[1]);
}

#endif
