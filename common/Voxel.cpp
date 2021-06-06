#include "Voxel.h"
#include "FastNoiseLite.h"

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
            FastNoiseLite noise;
            noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
            noise.SetFrequency(0.02f);
            voxel::ForEach(container, [&noise](int x, int y, int z, voxel::Voxel* voxel)
                {
                    auto v = noise.GetNoise((float)x, (float)z);
                    v = (v + 1) * 0.5f * voxel::Container::y;
                    voxel->type = y < v ? voxel::Type::Block : voxel::Type::Empty;
                });
        }

        void Process(Container& container, float step)
        {
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