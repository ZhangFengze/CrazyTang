#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include "../common/Move.h"

TEST_CASE("Move")
{
    ct::move::State origin;
    origin.velocity = { 0.f,0.f,1.f };
    origin.position = { 2.f,3.f,4.f };
    float dt = 1.f;
    auto newState = ct::move::Process(origin, 1.f);

    REQUIRE(newState.velocity == Eigen::Vector3f(0.f, 0.f, 1.f));
    REQUIRE(newState.position == Eigen::Vector3f(2.f, 3.f, 5.f));
}