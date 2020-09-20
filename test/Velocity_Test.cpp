#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include "../common/Velocity.h"
#include "../common/Archive.h"

TEST_CASE("default ctor")
{
    ct::Velocity Velocity;
    REQUIRE(Velocity.data.x()==0);
    REQUIRE(Velocity.data.y()==0);
    REQUIRE(Velocity.data.z()==0);
}

TEST_CASE("save & load")
{
    ct::Velocity origin;
    origin.data.x()=123;
    origin.data.y()=456;
    origin.data.z()=890.554f;

    ct::OutputStringArchive out;
    ct::ArchiveVelocity(out,origin);

    ct::InputStringArchive in(out.String());
    ct::Velocity loaded;
    ct::LoadVelocity(in,loaded);

    REQUIRE(origin.data.x()==loaded.data.x());
    REQUIRE(origin.data.y()==loaded.data.y());
    REQUIRE(origin.data.z()==loaded.data.z());
}