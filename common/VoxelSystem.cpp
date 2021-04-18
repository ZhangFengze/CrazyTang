#include "VoxelSystem.h"
#include "Voxel.h"

namespace ct
{
    namespace voxel_system
    {
        void GenerateVoxels(EntityContainer& entities)
        {
            for(int x=0;x<10;++x)
            {
                for(int y=0;y<10;++y)
                {
                    auto e=entities.Create();
                    auto voxel = e.Add<Voxel>();
                    voxel->index={x,y};
                    voxel->type=rand();
                }
            }
        }

        void Process(EntityContainer& entities, float step)
        {
            entities.ForEach([step](EntityHandle e)
                {
                    if (!e.Has<Voxel>())
                        return;
                    auto voxel = e.Get<Voxel>();
                    voxel->type=rand();
                });
        }
    }
}