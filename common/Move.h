#pragma once
#include <Eigen/Eigen>

namespace ct
{
    namespace move
    {
        struct State
        {
            Eigen::Vector3f velocity;
            Eigen::Vector3f position;
        };

        State Process(const State&, float dt);
    }
}