#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include "../common/Position.h"
#include "../common/Archive.h"

TEST_CASE("default ctor")
{
    ct::Position position;
    REQUIRE(position.data.x()==0);
    REQUIRE(position.data.y()==0);
    REQUIRE(position.data.z()==0);
}

TEST_CASE("save & load")
{
    ct::Position origin;
    origin.data.x()=123;
    origin.data.y()=456;
    origin.data.z()=890.554f;

    ct::OutputStringArchive out;
    ct::ArchivePosition(out,origin);

    ct::InputStringArchive in(out.String());
    ct::Position loaded;
    ct::LoadPosition(in,loaded);

    REQUIRE(origin.data.x()==loaded.data.x());
    REQUIRE(origin.data.y()==loaded.data.y());
    REQUIRE(origin.data.z()==loaded.data.z());
}