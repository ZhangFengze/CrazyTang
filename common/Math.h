#pragma once
#include <Eigen/Eigen>
#include <ZSerializer.hpp>

namespace ct
{
    float Rand(float min, float max);

    template<typename T>
    std::tuple<T, T, T> ToTuple(const Eigen::Matrix<T, 3, 1>& vec3)
    {
        return { vec3.x(), vec3.y(), vec3.z() };
    }
}

namespace zs
{
    template<>
    struct Trait<Eigen::Vector3f> : public WriteBitwise<Eigen::Vector3f>, public ReadBitwise<Eigen::Vector3f>
    {};
}