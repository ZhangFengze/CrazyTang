#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include "../common/Position.h"
#include "../common/Archive.h"

TEST_CASE("default ctor")
{
    ct::Position position;
    REQUIRE(position.x==0);
    REQUIRE(position.y==0);
    REQUIRE(position.z==0);
}

TEST_CASE("save & load")
{
    ct::Position origin;
    origin.x=123;
    origin.y=456;
    origin.z=890.554f;

    ct::OutputStringArchive out;
    ct::ArchivePosition(out,origin);

    ct::InputStringArchive in(out.String());
    ct::Position loaded;
    ct::LoadPosition(in,loaded);

    REQUIRE(origin.x==loaded.x);
    REQUIRE(origin.y==loaded.y);
    REQUIRE(origin.z==loaded.z);
}