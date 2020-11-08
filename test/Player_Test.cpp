#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include "../common/Player.h"
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

TEST_CASE("LoadPlayer should overwrite existing data")
{
    ct::EntityContainer container;
    auto origin = container.Create();
    ct::InitPlayer(origin);
    origin.Get<ct::Position>()->data = { 123.f,456.f,789.f };

    ct::OutputStringArchive out;
    ct::ArchivePlayer(out, origin);

    ct::InputStringArchive in{ out.String() };
    auto victim = container.Create();
    ct::InitPlayer(victim);
    victim.Get<ct::Position>()->data = { 1.f,1.f,1.f };

    ct::LoadPlayer(in, victim);

    REQUIRE(victim.Get<ct::Position>()->data == Eigen::Vector3f(123.f,456.f,789.f));
}

TEST_CASE("LoadPlayer should not modify irrelevant existing data")
{
    ct::EntityContainer container;
    auto origin = container.Create();
    ct::InitPlayer(origin);
    origin.Get<ct::Position>()->data = { 123.f,456.f,789.f };

    ct::OutputStringArchive out;
    ct::ArchivePlayer(out, origin);

    ct::InputStringArchive in{ out.String() };
    auto victim = container.Create();
    ct::InitPlayer(victim);
    auto f=victim.Add<float>();
    *f=3.f;
    victim.Get<ct::Position>()->data = { 1.f,1.f,1.f };

    ct::LoadPlayer(in, victim);

    REQUIRE(victim.Has<float>());
    REQUIRE(*victim.Get<float>() == 3.f);
}