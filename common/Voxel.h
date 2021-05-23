#pragma once
#include "../common/Position.h"
#include <ZSerializer.hpp>
#include <Eigen/Eigen>
#include <tuple>

namespace ct
{
    namespace voxel
    {
        constexpr float sideLength = 10.f;

        enum class Type
        {
            Unknown,
            Empty,
            Block
        };

        struct Voxel
        {
            Type type;
        };

        struct Container
        {
            static constexpr size_t x = 128;
            static constexpr size_t y = 16;
            static constexpr size_t z = 128;
            std::vector<Voxel> voxels{ x * y * z };

            Voxel* Get(int _x, int _y, int _z);
        };

        void GenerateVoxels(Container&);
        void Process(Container&, float step);
        std::tuple<int, int, int> DecodeIndex(const Position&);
    }
}

namespace zs
{
    template<>
    struct Trait<ct::voxel::Container>
        :public WriteMembers<ct::voxel::Container>, public ReadMembers<ct::voxel::Container>
    {
        static constexpr auto members = std::make_tuple
        (
            &ct::voxel::Container::voxels
        );
    };
}