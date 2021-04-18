#pragma once
#include "Entity.h"

namespace ct
{
    namespace voxel_system
    {
        void GenerateVoxels(EntityContainer&);
        void Process(EntityContainer&, float step);
    }
}