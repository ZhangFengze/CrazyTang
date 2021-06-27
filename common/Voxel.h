#pragma once
#include "../common/Position.h"
#include <ZSerializer.hpp>
#include <Eigen/Eigen>
#include <tuple>
#include <functional>

namespace ct
{
    namespace voxel
    {
        bool _InRange(int value, int begin, int end);

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

        template<typename Voxel>
        struct ContainerT
        {
            static constexpr size_t x = 128;
            static constexpr size_t y = 16;
            static constexpr size_t z = 128;
            static constexpr size_t size = x * y * z;

            static inline const auto indices = []
            {
                std::array<Eigen::Vector3i, size> indices;
                size_t index = 0;
                for (int _x = 0;_x < x;++_x)
                    for (int _y = 0;_y < y;++_y)
                        for (int _z = 0;_z < z;++_z)
                            indices[index++] = { _x,_y,_z };
                return indices;
            }();

            std::vector<Voxel> voxels{ x * y * z };

            Voxel* Get(int _x, int _y, int _z)
            {
                if (!ct::voxel::_InRange(_x, 0, x))
                    return nullptr;
                if (!ct::voxel::_InRange(_y, 0, y))
                    return nullptr;
                if (!ct::voxel::_InRange(_z, 0, z))
                    return nullptr;
                return GetNoCheck(_x, _y, _z);
            }

            Voxel* GetNoCheck(int _x, int _y, int _z)
            {
                size_t index = _x + _y * x + _z * x * y;
                return &voxels[index];
            }

            template<typename Func>
            void ForEach(Func func)
            {
                for (size_t _z = 0;_z < z;++_z)
                    for (size_t _y = 0;_y < y;++_y)
                        for (size_t _x = 0;_x < x;++_x)
                            func(_x, _y, _z, GetNoCheck(_x, _y, _z));
            }
        };

        using Container = ContainerT<Voxel>;
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