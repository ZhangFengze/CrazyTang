#pragma once
#include <ZSerializer.hpp>
#include <Eigen/Eigen>

namespace ct
{
    namespace voxel
    {
        struct Voxel
        {
            int type;
        };

        struct Container
        {
            static constexpr size_t x=32;
            static constexpr size_t y=32;
            static constexpr size_t z=4;
            std::vector<Voxel> voxels{x*y*z};

            Voxel* Get(int _x, int _y, int _z);
        };

        void GenerateVoxels(Container&);
        void Process(Container&, float step);
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