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
    struct Trait<xy::Name> : public WriteMembers<xy::Name>, public ReadMembers<xy::Name>
    {
        static constexpr auto members = std::make_tuple
        (
            &xy::Name::data
        );
    };
}