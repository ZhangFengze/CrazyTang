#pragma once
#include <Eigen/Eigen>
namespace xy
{
    struct Name 
    {
        std::string data = "";
    };
}

namespace zs
{
    template<>
    struct Trait<xy::Name> : public WriteBitwise<xy::Name>, public ReadBitwise<xy::Name>
    {};
}