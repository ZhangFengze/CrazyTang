#include "Player.h"

namespace ct
{
    void InitPlayer(EntityHandle e)
    {
        auto position=e.Add<Position>();
        position->data.x()=0;
        position->data.y()=0;
        position->data.z()=0;
    }
}