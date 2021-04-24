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
            size_t index = _x + _y * x + _z * x * y;
            return &voxels[index];
        }

        void GenerateVoxels(Container& container)
        {
        }

        static float accumulated = 0.f;
        void Process(Container& container, float step)
        {
            accumulated += step;
            for (size_t x = 0;x < container.x;++x)
            {
                for (size_t y = 0;y < container.y;++y)
                {
                    for (size_t z = 0;z < container.z;++z)
                    {
                        float altitude = std::sin(accumulated + x + y) * 2.f + 2.f;
                        if (z < altitude)
                            container.Get(x, y, z)->type = 1;
                        else
                            container.Get(x, y, z)->type = 0;
                    }
                }
            }
        }

        std::tuple<int, int, int> DecodeIndex(const Position& pos)
        {
            float side = 100.f;
            int x = pos.data.x() / side;
            int y = pos.data.y() / side;
            int z = pos.data.z() / side;
            return { x,y,z };
        }
    }
}