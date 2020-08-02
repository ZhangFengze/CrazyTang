#ifdef CT_TEST
#include <catch.hpp>
#include "Entity.h"

namespace
{
	struct Data {};
}

TEST_CASE("entity")
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
#endif
