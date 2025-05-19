#pragma once
#include <time.h>
#include <stdlib.h>

namespace Random
{   
    inline void SetSeed(long seed)
    {
        srand(seed);
    }

    inline int Range(int minInclusive, int maxExclusive)
    {
        return rand() % (maxExclusive - minInclusive) + minInclusive;
    }

    inline void InitRandom()
    {
        SetSeed(time(NULL));
    }

    template<typename T>
    inline T Choose(const std::vector<T>& vec)
    {
        if (vec.empty())
            Debug::Error("Random::Choose: Empty vector");

        int idx = Range(0, static_cast<int>(vec.size()));
        return vec[idx];
    }
}