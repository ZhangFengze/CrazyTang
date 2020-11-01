#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include "../common/Entity.h"
#include "../common/MoveSystem.h"
#include "../common/Position.h"
#include "../common/Velocity.h"

TEST_CASE("dummy")
{
    ct::EntityContainer entities;
    auto e = entities.Create();
    auto position = e.Add<ct::Position>();
    position->data = { 1.f,2.f,3.f };
    auto velocity = e.Add<ct::Velocity>();
    velocity->data = { 3.f,4.f,5.f };
    ct::move_system::Process(entities, 0.1f);
}

TEST_CASE("can handle entities without required components")
{
    ct::EntityContainer entities;
    auto e = entities.Create();
    ct::move_system::Process(entities, 0.1f);
}