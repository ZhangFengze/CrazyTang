#pragma once
#include <ZSerializer.hpp>
#include <Eigen/Eigen>

namespace ct
{
    struct Voxel
    {
        Eigen::Vector2i index;
        int type;
    };
}

namespace zs
{
    template<>
    struct Trait<ct::Voxel> : public WriteBitwise<ct::Voxel>, public ReadBitwise<ct::Voxel>
    {};
}