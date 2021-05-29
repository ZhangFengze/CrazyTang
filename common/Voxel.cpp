#include "Voxel.h"

namespace
{
    bool InRange(int value, int begin, int end)
    {
        return value >= begin && value < end;
    }
}

namespace ct
{
    namespace voxel
    {
        Voxel* Container::Get(int _x, int _y, int _z)
        {
            if (!InRange(_x, 0, x))
                return nullptr;
            if (!InRange(_y, 0, y))
                return nullptr;
            if (!InRange(_z, 0, z))
                return nullptr;
            return GetNoCheck(_x, _y, _z);
        }

        Voxel* Container::GetNoCheck(int _x, int _y, int _z)
        {
            size_t index = _x + _y * x + _z * x * y;
            return &voxels[index];
        }

        void GenerateVoxels(Container& container)
        {
            Process(container, 0.f);
        }

        static float accumulated = 0.f;
        void Process(Container& container, float step)
        {
            accumulated += step * 20.f;
            ForEach(container, [](int x, int y, int z, Voxel* voxel)
                {
                    float altitude = std::sin(accumulated + x + z) * 8.f + 8.f;
                    if (y < altitude)
                        voxel->type = Type::Block;
                    else
                        voxel->type = Type::Empty;
                });
        }

        std::tuple<int, int, int> DecodeIndex(const Position& pos)
        {
            int x = pos.data.x() / sideLength;
            int y = pos.data.y() / sideLength;
            int z = pos.data.z() / sideLength;
            return { x,y,z };
        }
    }
}