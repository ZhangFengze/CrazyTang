#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include "../common/Packet.h"

namespace
{
	const std::string testData = "hohoho hahaha";
}

TEST_CASE("default packet")
{
	ct::Packet p;
	REQUIRE(p.Size() == 0);
}

TEST_CASE("packet with size")
{
	ct::Packet p{ 1024 };
	REQUIRE(p.Size() == 1024);
}

TEST_CASE("packet with data")
{
	ct::Packet p{ testData.data(),testData.size() };
	REQUIRE(p.Size() == testData.size());
	REQUIRE(0 == memcmp(p.Data(), testData.data(), p.Size()));
}

TEST_CASE("packet should copy origin data")
{
	auto origin = testData;
	ct::Packet p{ origin.data(),origin.size() };
	origin[2] = 'a';

	REQUIRE(p.Size() == testData.size());
	REQUIRE(0 == memcmp(p.Data(), testData.data(), p.Size()));
}

TEST_CASE("packet comparison")
{
	ct::Packet p0{ testData.data(),testData.size() };
	ct::Packet p1{ testData.data(),testData.size() };
	REQUIRE(p0 == p1);

	char* raw = const_cast<char*>(p0.Data());
	raw[2] = 'a';
	REQUIRE(p0 != p1);
}

TEST_CASE("packet should deep copy")
{
	ct::Packet p0{ testData.data(),testData.size() };
	ct::Packet p1{ p0 };

	char* raw = const_cast<char*>(p0.Data());
	raw[2] = 'a';

	REQUIRE(p0 != p1);
	REQUIRE(p1.Size() == testData.size());
	REQUIRE(0 == memcmp(p1.Data(), testData.data(), p1.Size()));
}

TEST_CASE("packet should move rvalue")
{
	ct::Packet p0{ testData.data(),testData.size() };
	ct::Packet p1{ std::move(p0) };

	REQUIRE(p0.Size() == 0);
	REQUIRE(p1.Size() == testData.size());

	ct::Packet p2;
	p2 = std::move(p1);
	REQUIRE(p1.Size() == 0);
	REQUIRE(p2.Size() == testData.size());
}