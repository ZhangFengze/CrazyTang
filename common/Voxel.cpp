#include "Voxel.h"

namespace ct
{
    namespace voxel
    {
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
                            container.Get(x, y, z).type = 1;
                        else
                            container.Get(x, y, z).type = 0;
                    }
                }
            }
        }
    }
}