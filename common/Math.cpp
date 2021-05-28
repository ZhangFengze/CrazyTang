#include "Math.h"
#include <random>
namespace
{
    std::random_device rd;
    std::mt19937 gen(rd());
}
namespace ct
{
    float Rand(float min, float max)
    {
        std::uniform_real_distribution<float> dis(min, max);
        return dis(gen);
    }
}