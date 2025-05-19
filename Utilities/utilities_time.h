#pragma once
#include <SDL2/SDL.h>

namespace Time
{
    static Uint64 previous = 0;
    static float delta = 0.0f;

    static constexpr int   FPS_SAMPLES = 100;
    static float           fpsBuffer[FPS_SAMPLES] = {0};
    static int             fpsIndex   = 0;
    static bool            fpsFilled  = false;

    void Init()
    {
        previous = SDL_GetTicks64();
        delta    = 0.0f;
        fpsIndex = 0;
        fpsFilled = false;
        for (int i = 0; i < FPS_SAMPLES; ++i) fpsBuffer[i] = 0.0f;
    }

    void Update()
    {
        Uint64 now   = SDL_GetTicks64();
        delta        = (now - previous) * 0.001f;
        previous     = now;

        float currentFPS = (delta > 0.0f) ? (1.0f / delta) : 0.0f;
        fpsBuffer[fpsIndex] = currentFPS;
        fpsIndex = (fpsIndex + 1) % FPS_SAMPLES;
        if (fpsIndex == 0) fpsFilled = true;

    }

    float DeltaTime()
    {
        return delta;
    }

    inline float Time()
    {
        return SDL_GetTicks64() / 1000.f;
    }


    float GetAverageFPS()
    {
        int count = fpsFilled ? FPS_SAMPLES : fpsIndex;
        if (count == 0) return 0.0f;
        float sum = 0.0f;
        for (int i = 0; i < count; ++i)
            sum += fpsBuffer[i];
        return sum / static_cast<float>(count);
    }
}