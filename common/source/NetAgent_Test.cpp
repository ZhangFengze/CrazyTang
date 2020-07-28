#ifdef CT_TEST
#include <catch.hpp>
#include "NetAgent.h"
#include "Pipe.h"
#include "Serializer.h"

using namespace std::literals;

namespace
{
	const std::string tag = "tag";
	const std::string content = "content";
	const std::string anotherTag = "tag2";
}

TEST_CASE("net agent has listener")
{
	auto pipe = std::make_shared<ct::MockPipe>();
	ct::NetAgent agent{ pipe };
	std::string received;
	agent.Listen(tag, [&](std::string&& content)
	{
		received = content;
	});

	ct::OutputStringArchive ar;
	ar.Write(tag);
	ar.Write(content);
	pipe->PacketArrive(ct::Packet(ar.String()));

	REQUIRE(received == content);
}

TEST_CASE("net agent has no listener")
{
	auto pipe = std::make_shared<ct::MockPipe>();
	ct::NetAgent agent{ pipe };
	bool called = false;
	agent.Listen(tag, [&](std::string&& content)
	{
		called = true;
	});

	ct::OutputStringArchive ar;
	ar.Write(anotherTag);
	ar.Write(content);
	pipe->PacketArrive(ct::Packet(ar.String()));

	REQUIRE(!called);
}

TEST_CASE("net agent pipe broken")
{
	auto pipe = std::make_shared<ct::MockPipe>();
	ct::NetAgent agent{ pipe };
	bool called = false;
	agent.OnError([&]()
	{
		called = true;
	});

	pipe->SetBroken();

	REQUIRE(called);
}
#endif
