#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include "../common/Entity.h"
#include "../common/MoveSystem.h"

TEST_CASE("dummy")
{
    ct::EntityContainer entities;
    ct::move_system::Process(entities, 0.1f);
}

TEST_CASE("can handle entities without required components")
{
    ct::EntityContainer entities;
    auto e = entities.Create();
    ct::move_system::Process(entities, 0.1f);
}