#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>
#include <string>
#include <memory>

class MainMenu : public Object {
public:
MainMenu()
    {
        SetPosition(0.0f, 0.0f);
        delimiterAffectedByRotation = false;
        collision = false;
    }

    void Init(SDL_Renderer* renderer)
    {
        mainMenuAnimationSystem = std::make_unique<AnimationSystem>(renderer, "MainMenu/");
        
        size_t frame_count_1 = 42, frame_count_2 = 22;
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
            paths1.push_back("start/menu_" + number + ".png");
        }
        for (int i = 0; i < frame_count_2; ++i)
        {
            if (i == 0) number = std::string("000");
            else if (i < 10) number = std::string("00") + std::to_string(i);
            else number = std::string("0") + std::to_string(i);
            paths2.push_back("loop/menu_loop_" + number + ".png");
        }
        mainMenuAnimationSystem->SetEntries({  { "start", paths1, 0.075f },   { "loop", paths2, 0.075f } });
        mainMenuAnimationSystem->SetAnimation("start", false);
    }

    void Update(float deltaTime) override
    {
        if (mainMenuAnimationSystem !=nullptr && mainMenuAnimationSystem->CurrentId()=="start"&&mainMenuAnimationSystem->Finished())
        {
            mainMenuAnimationSystem->SetAnimation("loop", true);
        }

        if (Input::GetKeyDown(SDL_SCANCODE_SPACE))
        {
            SoundManager::Instance().StopAll();
            SoundManager::Instance().PlaySound("dojo_ost", true, 0.5f);
            Scene::Instance().NextLevel();
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
        if (!debug_show || mainMenuAnimationSystem == nullptr) return;

        SDL_Texture* tex = mainMenuAnimationSystem->Next(Time::DeltaTime());

        int w, h;
        SDL_GetRendererOutputSize(renderer, &w, &h);

        SDL_Rect rect;
        rect.w = w;
        rect.h = h;
        rect.x = 0;
        rect.y = 0;
        SDL_RenderCopy(renderer, tex, nullptr, &rect);
    }

private:
    std::unique_ptr<AnimationSystem> mainMenuAnimationSystem;
    bool debug_show = true;
};
