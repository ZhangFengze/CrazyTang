#pragma once
#include <ZSerializer.hpp>
#include "../common/Entity.h"
#include "../common/Position.h"
#include "../common/Velocity.h"

namespace ct
{
    void InitPlayer(EntityHandle);

    template<typename Out>
    void ArchivePlayer(Out& out, EntityHandle e)
    {
        zs::Write(out, *e.Get<Position>());
        zs::Write(out, *e.Get<Velocity>());
    }

    template<typename In>
    void LoadPlayer(In& in, EntityHandle e)
    {
        if (!e.Has<Position>())
            e.Add<Position>();
        auto pos = zs::Read<Position>(in);
        *e.Get<Position>()=std::get<0>(pos);

        if (!e.Has<Velocity>())
            e.Add<Velocity>();
        auto vel = zs::Read<Velocity>(in);
        *e.Get<Velocity>()=std::get<0>(vel);
    }
}