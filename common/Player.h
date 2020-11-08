#pragma once
#include "../common/Entity.h"
#include "../common/Position.h"
#include "../common/Velocity.h"

namespace ct
{
    void InitPlayer(EntityHandle);

    template<typename OutputArchive>
    void ArchivePlayer(OutputArchive& ar,EntityHandle e)
    {
        ComponentTraits<Position>::Archive(ar,*e.Get<Position>());
        ComponentTraits<Velocity>::Archive(ar,*e.Get<Velocity>());
    }

    template<typename InputArchive>
    void LoadPlayer(InputArchive& ar,EntityHandle e)
    {
        if(!e.Has<Position>())
            e.Add<Position>();
        ComponentTraits<Position>::Load(ar,*e.Get<Position>());
        if(!e.Has<Velocity>())
            e.Add<Velocity>();
        ComponentTraits<Velocity>::Load(ar,*e.Get<Velocity>());
    }
}