#pragma once
#include "../common/Entity.h"
#include "../common/Position.h"

namespace ct
{
    void InitPlayer(EntityHandle);

    template<typename OutputArchive>
    void ArchivePlayer(OutputArchive& ar,EntityHandle e)
    {
        ArchivePosition(ar,*e.Get<Position>());
    }

    template<typename InputArchive>
    void LoadPlayer(InputArchive& ar,EntityHandle e)
    {
        e.Add<Position>();
        LoadPosition(ar,*e.Get<Position>());
    }
}