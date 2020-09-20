#pragma once
#include <Eigen/Eigen>
namespace ct
{
    struct Position
    {
        Eigen::Vector3f data=Eigen::Vector3f::Zero();
    };

    template<typename OutputArchive>
    void ArchivePosition(OutputArchive& ar, const Position& position)
    {
        ar.Write(position);
    }

    template<typename InputArchive>
    void LoadPosition(InputArchive& ar, Position& position)
    {
        ar.Read(position);
    }
}