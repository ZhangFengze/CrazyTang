#pragma once
#include <Eigen/Eigen>
#include <ZSerializer.hpp>
namespace ct
{
    struct Position
    {
        Eigen::Vector3f data=Eigen::Vector3f::Zero();
    };
}

namespace zs
{
    template<>
    struct Trait<ct::Position> : public WriteBitwise<ct::Position>, public ReadBitwise<ct::Position>
    {};
}