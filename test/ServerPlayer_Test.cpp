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
    origin.Get<ct::Position>()->x=123;
    origin.Get<ct::Position>()->y=456;
    origin.Get<ct::Position>()->z=456;

    ct::OutputStringArchive out;
    ct::ArchivePlayer(out,origin);

    ct::InputStringArchive in{out.String()};
    auto loaded=container.Create();
    ct::LoadPlayer(in,loaded);

    REQUIRE(loaded.Get<ct::Position>()->x==origin.Get<ct::Position>()->x);
    REQUIRE(loaded.Get<ct::Position>()->y==origin.Get<ct::Position>()->y);
    REQUIRE(loaded.Get<ct::Position>()->z==origin.Get<ct::Position>()->z);
    
    REQUIRE(true);
}