#pragma once
namespace ct
{
    struct Position
    {
        float x=0;
        float y=0;
        float z=0;
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