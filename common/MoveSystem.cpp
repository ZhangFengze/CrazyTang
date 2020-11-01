#include "MoveSystem.h"
#include "Move.h"
#include "Position.h"
#include "Velocity.h"
namespace ct
{
    namespace move_system
    {
        void Process(EntityContainer& entities, float step)
        {
            entities.ForEach([step](EntityHandle e)
                {
                    move::State s;
                    s.position = e.Get<Position>()->data;
                    s.velocity = e.Get<Velocity>()->data;
                    s = move::Process(s, step);
                    e.Get<Position>()->data = s.position;
                    e.Get<Velocity>()->data = s.velocity;
                });
        }
    }
}