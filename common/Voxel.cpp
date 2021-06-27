#include "Voxel.h"
#include "FastNoiseLite.h"

namespace ct
{
    namespace voxel
    {
        bool _InRange(int value, int begin, int end)
        {
            return value >= begin && value < end;
        }

        void GenerateVoxels(Container& container)
        {
            FastNoiseLite noise;
            noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
            noise.SetFrequency(0.02f);
            container.ForEach([&noise](int x, int y, int z, voxel::Voxel* voxel)
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