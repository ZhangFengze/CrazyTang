#ifdef CT_TEST
#include <catch.hpp>
#include "Packet.h"
#include "Socket.h"
#include "Pipe.h"

using namespace std::literals;

namespace
{
	const ct::Packet packet0 = "hello world"sv;
	const ct::Packet packet1 = "no bugs"sv;
	const ct::Packet packet2 = "long story"sv;
	const ct::Packet packet3 = "stop!"sv;
}

TEST_CASE("pipe construction")
{
	ct::Pipe<ct::StubSocket> pipe{ ct::StubSocket{} };
}

TEST_CASE("pipe write packet")
{
	asio::io_context io;
	ct::Pipe<ct::MockSocket> pipe{ ct::MockSocket{io} };

	pipe.SendPacket(packet0);
	pipe.SendPacket(packet1);

	io.poll();

	REQUIRE(pipe.socket_.writtenPackets.size() == 2);

	REQUIRE(packet0 == pipe.socket_.writtenPackets[0]);
	REQUIRE(packet1 == pipe.socket_.writtenPackets[1]);
}

TEST_CASE("pipe read packet")
{
	asio::io_context io;
	ct::Pipe<ct::MockSocket> pipe{ ct::MockSocket{io} };

	std::vector<ct::Packet> packets;
	pipe.OnPacket([&packets](ct::Packet&& packet)
	{
		packets.push_back(packet);
	});

	pipe.socket_.PacketArrive({}, packet0);
	pipe.socket_.PacketArrive({}, packet1);

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
	pipe.OnPacket([](ct::Packet&&) {});
	pipe.OnBroken([&broken]()
	{
		REQUIRE(!broken);
		broken = true;
	});

	pipe.socket_.PacketArrive({}, packet0);
	pipe.socket_.PacketArrive({}, packet1);
	pipe.socket_.PacketArrive(asio::error::eof, {});

	io.poll();

	REQUIRE(broken);
	REQUIRE(pipe.IsBroken());
}

TEST_CASE("pipe should stop working after broken")
{
	asio::io_context io;
	ct::Pipe<ct::MockSocket> pipe{ ct::MockSocket{io} };

	bool broken = false;
	std::vector<ct::Packet> packets;
	pipe.OnPacket([&packets,&broken](ct::Packet&& packet)
	{
		REQUIRE(!broken);
		packets.push_back(packet);
	});
	pipe.OnBroken([&broken]()
	{
		REQUIRE(!broken);
		broken = true;
	});

	pipe.socket_.PacketArrive({}, packet0);
	pipe.socket_.PacketArrive({}, packet1);
	pipe.socket_.PacketArrive(asio::error::eof, {});
	pipe.socket_.PacketArrive({}, packet2);

	pipe.SendPacket(packet3);

	io.poll();

	REQUIRE(packets.size() == 2);
	REQUIRE(broken);
	REQUIRE(pipe.socket_.writtenPackets.empty());
}

TEST_CASE("pipe change packet handler")
{
	asio::io_context io;
	ct::Pipe<ct::MockSocket> pipe{ ct::MockSocket{io} };

	std::vector<ct::Packet> packets0;
	pipe.OnPacket([&packets0](ct::Packet&& packet)
	{
		packets0.push_back(packet);
	});

	pipe.socket_.PacketArrive({}, packet0);
	pipe.socket_.PacketArrive({}, packet1);

	io.poll();

	std::vector<ct::Packet> packets1;
	pipe.OnPacket([&packets1](ct::Packet&& packet)
	{
		packets1.push_back(packet);
	});
	
	pipe.socket_.PacketArrive({}, packet2);
	pipe.socket_.PacketArrive({}, packet3);

	io.poll();

	REQUIRE(packets0.size() == 2);
	REQUIRE(packet0 == packets0[0]);
	REQUIRE(packet1 == packets0[1]);

	REQUIRE(packets1.size() == 2);
	REQUIRE(packet2 == packets1[0]);
	REQUIRE(packet3 == packets1[1]);
}

#endif
