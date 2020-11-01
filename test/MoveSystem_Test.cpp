#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include "../common/Entity.h"
#include "../common/MoveSystem.h"

TEST_CASE("dummy")
{
    ct::EntityContainer entities;
    ct::move_system::Process(entities, 0.1f);
}