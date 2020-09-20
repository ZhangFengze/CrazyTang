#pragma once
#include <Eigen/Eigen>
#include "ComponentTraits.h"
namespace ct
{
    struct Velocity
    {
        Eigen::Vector3f data=Eigen::Vector3f::Zero();
    };

    template<>
    struct ComponentTraits<Velocity>: public BitwiseComponentTraits<Velocity>
    {};
}