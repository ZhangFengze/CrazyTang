#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include "../server/Player.h"
#include "../common/Entity.h"
#include "../common/Archive.h"

TEST_CASE("dummy init")
{
    ct::EntityContainer container;
    auto e=container.Create();
    ct::InitPlayer(e);
    REQUIRE(true);
}

TEST_CASE("save & load")
{
    ct::EntityContainer container;
    auto origin=container.Create();
    ct::InitPlayer(origin);
    origin.Get<ct::Position>()->data.x()=123;
    origin.Get<ct::Position>()->data.y()=456;
    origin.Get<ct::Position>()->data.z()=456;

    ct::OutputStringArchive out;
    ct::ArchivePlayer(out,origin);

    ct::InputStringArchive in{out.String()};
    auto loaded=container.Create();
    ct::LoadPlayer(in,loaded);

    REQUIRE(loaded.Get<ct::Position>()->data.x()==origin.Get<ct::Position>()->data.x());
    REQUIRE(loaded.Get<ct::Position>()->data.y()==origin.Get<ct::Position>()->data.y());
    REQUIRE(loaded.Get<ct::Position>()->data.z()==origin.Get<ct::Position>()->data.z());
    
    REQUIRE(true);
}