#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include "../common/Entity.h"

namespace
{
	struct Data {};
}

TEST_CASE("entity default constructor")
{
	ct::EntityHandle e;
	REQUIRE(!e.Valid());
}

TEST_CASE("entity component")
{
	ct::EntityContainer entities;
	ct::EntityHandle e = entities.Create();

	REQUIRE(e.Has<Data>() == false);
	REQUIRE(e.Get<Data>() == nullptr);

	auto data = e.Add<Data>();
	REQUIRE(e.Has<Data>() == true);
	REQUIRE(e.Get<Data>() == data);

	e.Remove<Data>();
	REQUIRE(e.Has<Data>() == false);
	REQUIRE(e.Get<Data>() == nullptr);
}

TEST_CASE("entity equality")
{
	ct::EntityContainer entities;
	ct::EntityHandle e0 = entities.Create();
	ct::EntityHandle e1 = entities.Create();
	REQUIRE(e0 == e0);
	REQUIRE(e0 != e1);

	auto e0Copy = e0;
	REQUIRE(e0Copy == e0);
	REQUIRE(e0Copy != e1);

	ct::EntityHandle defaultConstructed;
	REQUIRE(defaultConstructed != e0);
	REQUIRE(defaultConstructed != e1);
}

TEST_CASE("entity comparison")
{
	ct::EntityContainer entities;
	ct::EntityHandle e0 = entities.Create();
	ct::EntityHandle e1 = entities.Create();
	REQUIRE((e0 < e1) != (e1 < e0));
}

TEST_CASE("entity hash")
{
	ct::EntityContainer entities;
	ct::EntityHandle e = entities.Create();
	REQUIRE(std::hash<ct::EntityHandle>{}(e) == std::hash<ct::EntityHandle>{}(e));
}

TEST_CASE("entity destroy")
{
	ct::EntityContainer entities;

	ct::EntityHandle e = entities.Create();
	REQUIRE(e.Valid());
	e.Destroy();
	REQUIRE(e.Valid() == false);
}

TEST_CASE("for each")
{
	ct::EntityContainer container;

	std::set<ct::EntityHandle> entities;
	for (int i = 0;i < 10;++i)
		entities.emplace(container.Create());

	container.ForEach([&entities](ct::EntityHandle e)
		{
			REQUIRE(!entities.empty());
			entities.erase(e);
		}
	);

	REQUIRE(entities.empty());
}