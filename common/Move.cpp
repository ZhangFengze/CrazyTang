#include "Move.h"
namespace ct
{
    namespace move
    {
        State Process(const State& origin, float dt)
        {
            auto copy = origin;
            copy.position += copy.velocity * dt;
            return copy;
        }
    }
}