#pragma once
#include <Eigen/Eigen>
#include <ZSerializer.hpp>

namespace ct
{
    float Rand(float min, float max);
}

namespace zs
{
    template<>
    struct Trait<Eigen::Vector3f> : public WriteBitwise<Eigen::Vector3f>, public ReadBitwise<Eigen::Vector3f>
    {};
}