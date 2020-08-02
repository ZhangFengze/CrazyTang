#ifdef CT_TEST
#include <catch.hpp>
#include "ECS.h"

namespace
{
	struct Data {};
}

TEST_CASE("entity")
{
	ct::Entity e;

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
