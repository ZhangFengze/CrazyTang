#include "VoxelWatcher.h"
#include "ConnectionInfo.h"
#include "../common/Position.h"
#include "ZSerializer.hpp"

using namespace ct;
namespace
{
    template<typename Func>
    void TraverseVoxels(voxel::Container& voxels, const Position& pos, int side, Func&& func)
    {
        auto [x, y, z] = voxel::DecodeIndex(pos);
        for (int nowX = x - side;nowX < x + side;++nowX)
        {
            for (int nowY = y - side;nowY < y + side;++nowY)
            {
                for (int nowZ = z - side;nowZ < z + side;++nowZ)
                {
                    auto voxel = voxels.Get(nowX, nowY, nowZ);
                    if (!voxel)
                        continue;
                    func(nowX, nowY, nowZ, *voxel);
                }
            }
        }
    }

    std::vector<std::string> ArchiveVoxels(voxel::Container& voxels, const Position& pos,
        int side, int voxelPerMessage)
    {
        std::vector<std::string> messages;
        zs::StringWriter out;
        int count = 0;

        TraverseVoxels(voxels, pos, side,
            [&](int x, int y, int z, const voxel::Voxel& voxel)
            {
                if (count >= voxelPerMessage)
                {
                    messages.emplace_back(out.String());
                    out = zs::StringWriter{};
                    count = 0;
                }
                zs::Write(out, x);
                zs::Write(out, y);
                zs::Write(out, z);
                zs::Write(out, voxel);
                ++count;
            });

        if (count != 0)
            messages.emplace_back(out.String());
        return messages;
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
                    auto messages = ArchiveVoxels(voxels, *pos, 50, 100);
                    for (const auto& message : messages)
                        agent->Send("voxels", message);
                });
        }
    }
}