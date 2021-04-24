#include "VoxelWatcher.h"
#include "ConnectionInfo.h"
#include "../common/Position.h"
#include "ZSerializer.hpp"

using namespace ct;
namespace
{
    std::string ArchiveVoxels(voxel::Container& voxels, const Position& pos)
    {
        zs::StringWriter out;
        auto [x, y, z] = voxel::DecodeIndex(pos);
        int side = 10;
        for (int nowX = x - side;nowX < x + side;++nowX)
        {
            for (int nowY = y - side;nowY < y + side;++nowY)
            {
                for (int nowZ = z - side;nowZ < z + side;++nowZ)
                {
                    auto voxel = voxels.Get(nowX, nowY, nowZ);
                    if (!voxel)
                        continue;
                    zs::Write(out, nowX);
                    zs::Write(out, nowY);
                    zs::Write(out, nowZ);
                    zs::Write(out, *voxel);
                }
            }
        }
        return out.String();
    }
}

namespace ct
{
    namespace voxel_watcher
    {
        void Process(EntityContainer& entities, voxel::Container& voxels, float step)
        {
            entities.ForEach([&](EntityHandle e)
                {
                    auto connection = e.Get<ConnectionInfo>();
                    if (!connection)
                        return;
                    auto pos = e.Get<Position>();
                    if (!pos)
                        return;
                    auto agent = connection->agent.lock();
                    assert(agent);
                    agent->Send("voxels", ArchiveVoxels(voxels, *pos));
                });
        }
    }
}