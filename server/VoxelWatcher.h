#pragma once
#include "../common/Voxel.h"
#include "../common/Entity.h"

namespace ct
{
    namespace voxel_watcher
    {
        void Process(EntityContainer&, voxel::Container&, float step);
        void Sync(EntityHandle&, voxel::Container&);
    }
}