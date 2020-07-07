#ifdef CT_TEST
#include <catch.hpp>
#include "MessageHandler.h"

TEST_CASE("constructor", "[MessageHandler]")
{
	ct::MessageHandler handler;
}

TEST_CASE("registerd message", "[MessageHandler]")
{
	ct::MessageHandler handler;
	bool called = false;
	handler.Register("hello",
		[&called](const std::string& message)
	{
		REQUIRE(message == "hello there");
		called = true;
	});
	handler.OnMessage("hello there");
	REQUIRE(called);
}

TEST_CASE("unregisterd message", "[MessageHandler]")
{
	ct::MessageHandler handler;
	bool called = false;
	handler.Register("hello",
		[&called](const std::string& message)
	{
		called = true;
	});
	handler.OnMessage("hi there");
	REQUIRE(!called);
}

TEST_CASE("unregister", "[MessageHandler]")
{
	ct::MessageHandler handler;
	bool called = false;
	handler.Register("hello",
		[&called](const std::string& message)
	{
		called = true;
	});
	handler.UnRegister("hello");
	handler.OnMessage("hello there");
	REQUIRE(!called);
}

#endif