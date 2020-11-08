#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include "../common/Player.h"
#include "../common/Entity.h"
#include "../common/Archive.h"

namespace
{
    const Eigen::Vector3f position{ 123.f,456.f,789.f };
    const Eigen::Vector3f position2{ 1.f,6.f,9.f };
}

TEST_CASE("dummy init")
{
    ct::EntityContainer container;
    auto e = container.Create();
    ct::InitPlayer(e);
    REQUIRE(true);
}

TEST_CASE("save & load")
{
    ct::EntityContainer container;
    auto origin = container.Create();
    ct::InitPlayer(origin);
    origin.Get<ct::Position>()->data = position;

    ct::OutputStringArchive out;
    ct::ArchivePlayer(out, origin);

    ct::InputStringArchive in{ out.String() };
    auto loaded = container.Create();
    ct::LoadPlayer(in, loaded);

    REQUIRE(loaded.Get<ct::Position>()->data == position);
}

TEST_CASE("LoadPlayer should overwrite existing data")
{
    ct::EntityContainer container;
    auto origin = container.Create();
    ct::InitPlayer(origin);
    origin.Get<ct::Position>()->data = position;

    ct::OutputStringArchive out;
    ct::ArchivePlayer(out, origin);

    ct::InputStringArchive in{ out.String() };
    auto victim = container.Create();
    ct::InitPlayer(victim);
    victim.Get<ct::Position>()->data = position2;

    ct::LoadPlayer(in, victim);

    REQUIRE(victim.Get<ct::Position>()->data == position);
}

TEST_CASE("LoadPlayer should not modify irrelevant existing data")
{
    ct::EntityContainer container;
    auto origin = container.Create();
    ct::InitPlayer(origin);
    origin.Get<ct::Position>()->data = position;

    ct::OutputStringArchive out;
    ct::ArchivePlayer(out, origin);

    ct::InputStringArchive in{ out.String() };
    auto victim = container.Create();
    ct::InitPlayer(victim);

    float* data = victim.Add<float>();
    *data = 123.f;

    ct::LoadPlayer(in, victim);

    REQUIRE(victim.Has<float>());
    REQUIRE(*victim.Get<float>() == 123.f);
}