#pragma once
#include <cstdint>
#include <atomic>

namespace ct
{
    struct UUID
    {
        uint64_t id;
    };

    // TODO
    inline uint64_t GenerateUUID()
    {
        static std::atomic_uint64_t id;
        return ++id;
    }
}