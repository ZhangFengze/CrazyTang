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
        ArchivePosition(ar,*e.Get<Position>());
        ArchiveVelocity(ar,*e.Get<Velocity>());
    }

    template<typename InputArchive>
    void LoadPlayer(InputArchive& ar,EntityHandle e)
    {
        e.Add<Position>();
        LoadPosition(ar,*e.Get<Position>());
        e.Add<Velocity>();
        LoadVelocity(ar,*e.Get<Velocity>());
    }
}