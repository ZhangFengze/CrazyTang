#pragma once
#include <Eigen/Eigen>
#include "ComponentTraits.h"
namespace ct
{
    struct Position
    {
        Eigen::Vector3f data=Eigen::Vector3f::Zero();
    };

    template<>
    struct ComponentTraits<Position>: public BitwiseComponentTraits<Position>
    {};
}