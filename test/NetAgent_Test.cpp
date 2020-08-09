#include <catch.hpp>
#include "../common/NetAgent.h"
#include "../common/Pipe.h"
#include "../common/Serializer.h"

using namespace std::literals;

namespace
{
	const std::string tag = "tag";
	const std::string content = "content";
	const std::string anotherTag = "tag2";

	struct CallFunctionWhenDestroy
	{
		CallFunctionWhenDestroy(std::function<void(void)> f) :func(f) {}
		~CallFunctionWhenDestroy()
		{
			func();
		}
		std::function<void(void)> func;
	};
}

TEST_CASE("net agent has listener")
{
	auto [pipe0, pipe1] = ct::PairedPipe::CreatePair();
	ct::NetAgent agent0{ pipe0 };
	ct::NetAgent agent1{ pipe1 };

	std::string received;
	agent0.Listen(tag, [&](std::string&& content)
	{
		received = content;
	});
	agent1.Send(tag, content);

	REQUIRE(received == content);
}

TEST_CASE("net agent has no listener")
{
	auto [pipe0, pipe1] = ct::PairedPipe::CreatePair();
	ct::NetAgent agent0{ pipe0 };
	ct::NetAgent agent1{ pipe1 };

	bool called = false;
	agent0.Listen(tag, [&](std::string&& content)
	{
		called = true;
	});
	agent1.Send(anotherTag, content);

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

TEST_CASE("net agent clean up after broken")
{
	auto pipe = std::make_shared<ct::MockPipe>();
	ct::NetAgent agent{ pipe };

	bool called = false;
	{
		auto call = std::make_shared<CallFunctionWhenDestroy>([&called]
		{
			called = true;
		});
		agent.OnError([call]()
		{
		});
	}

	pipe->SetBroken();

	REQUIRE(called == true);
}