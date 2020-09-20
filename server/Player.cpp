#include "Player.h"

namespace ct
{
    void InitPlayer(EntityHandle e)
    {
        auto position=e.Add<Position>();
        position->x=0;
        position->y=0;
        position->z=0;
    }
}