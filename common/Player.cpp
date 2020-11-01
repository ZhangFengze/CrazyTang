#include "Player.h"

namespace ct
{
    void InitPlayer(EntityHandle e)
    {
        auto position=e.Add<Position>();
        position->data.x()=0;
        position->data.y()=0;
        position->data.z()=0;

        auto velocity=e.Add<Velocity>();
        velocity->data.x()=0;
        velocity->data.y()=0;
        velocity->data.z()=0;
    }
}