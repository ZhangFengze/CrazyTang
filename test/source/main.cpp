#define CATCH_CONFIG_MAIN
#include <catch.hpp>

TEST_CASE("foo", "[bar]") 
{
	REQUIRE(1 == 1);
}