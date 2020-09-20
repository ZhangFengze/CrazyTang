#pragma once
namespace ct
{
    template<typename Component>
    struct ComponentTraits;

    template<typename Component>
    struct BitwiseComponentTraits
    {
        template<typename OutputArchive>
        static void Archive(OutputArchive& ar, const Component& component)
        {
            ar.Write(component);
        }

        template<typename InputArchive>
        static void Load(InputArchive& ar, Component& component)
        {
            ar.Read(component);
        }
    };
}