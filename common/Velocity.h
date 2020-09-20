#pragma once
#include <Eigen/Eigen>
namespace ct
{
    struct Velocity
    {
        Eigen::Vector3f data=Eigen::Vector3f::Zero();
    };

    template<typename OutputArchive>
    void ArchiveVelocity(OutputArchive& ar, const Velocity& velocity)
    {
        ar.Write(velocity);
    }

    template<typename InputArchive>
    void LoadVelocity(InputArchive& ar, Velocity& velocity)
    {
        ar.Read(velocity);
    }
}