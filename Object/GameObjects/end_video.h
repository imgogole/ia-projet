#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>
#include <string>
#include <memory>

class EndVideo : public Object {
public:
EndVideo()
    {
        SetPosition(0.0f, 0.0f);
        delimiterAffectedByRotation = false;
        collision = false;
    }

    void Init(SDL_Renderer* renderer)
    {
        video = std::make_unique<AnimationSystem>(renderer, "EndVideo/");
        
        size_t frame_count_1 = 40, frame_count_2 = 16;
        std::vector<std::string> paths1;
        std::vector<std::string> paths2;
        paths1.reserve(frame_count_1);
        paths2.reserve(frame_count_2);
        std::string number;
        for (int i = 0; i < frame_count_1; ++i)
        {
            if (i == 0) number = std::string("000");
            else if (i < 10) number = std::string("00") + std::to_string(i);
            else number = std::string("0") + std::to_string(i);
            paths1.push_back("endkill/floor_last_kill_" + number + ".png");
        }
        for (int i = 0; i < frame_count_2; ++i)
        {
            if (i == 0) number = std::string("000");
            else if (i < 10) number = std::string("00") + std::to_string(i);
            else number = std::string("0") + std::to_string(i);
            paths2.push_back("boss/zanka no tachi_" + number + ".png");
        }
        video->SetEntries({  { "end_kill", paths1, 0.08f },   { "boss", paths2, 0.1f } });
        video->SetAnimation("end_kill", false);

        timeBeforeExit = 0.0f;
    }

    void StartVideo(std::string id)
    {
        bool isBoss = id == "boss";
        video->ResetAnimation();
        video->SetAnimation(id, false);
        SoundManager::Instance().PlaySound(id, false, 1.0f);
        timeBeforeExit = isBoss ? 3.0f : 4.0f;
    }

    void Update(float deltaTime) override
    {
        if (!End())
        {
            timeBeforeExit -= deltaTime;
        }
    }

    void OnCollisionEnter(Object* collision) override
    {
    }

    void OnCollisionExit(Object* collision) override
    {
    }

    void Render(SDL_Renderer* renderer, const Vector2D& givenPosition) override
    {
        if (!debug_show || video == nullptr) return;

        if (End()) return;

        SDL_Texture* tex = video->Next(Time::DeltaTime());

        int w, h;
        SDL_GetRendererOutputSize(renderer, &w, &h);

        SDL_Rect rect;
        rect.w = w;
        rect.h = h;
        rect.x = 0;
        rect.y = 0;
        SDL_RenderCopy(renderer, tex, nullptr, &rect);
    }

    bool End()
    {
        return timeBeforeExit <= 0.0f;
    }

private:
    std::unique_ptr<AnimationSystem> video;
    bool debug_show = true;
    float timeBeforeExit = 0.0f;
};
