#pragma once
#include <Eigen/Eigen>
namespace ct
{
    struct Velocity
    {
        Eigen::Vector3f data=Eigen::Vector3f::Zero();
    };
}

namespace zs
{
    template<>
    struct Trait<ct::Velocity> : public WriteBitwise<ct::Velocity>, public ReadBitwise<ct::Velocity>
    {};
}