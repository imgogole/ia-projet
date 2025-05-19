#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>
#include <string>
#include <memory>

class Exit : public Object {
public:
    Exit()
    {
        SetPosition(0.0f, 0.0f);
        SetFlags(ObjectFlag::Flag_Exit);
        delimiterAffectedByRotation = false;
        collision = true;
        color = { 0, 255, 0, 0 };
    }

    void Init(int w, int h)
    {
        renderDelimiter.x = collisionDelimiter.x = w;
        renderDelimiter.y = collisionDelimiter.y = h;
    }

    void Update(float deltaTime) override
    {
    }

    void OnCollisionEnter(Object* collision) override
    {

    }

    void OnCollisionExit(Object* collision) override
    {
    }

    void Render(SDL_Renderer* renderer, const Vector2D& givenPosition) override
    {
        if (!debug_show) return;

        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_Rect rect;
        rect.w = static_cast<int>(renderDelimiter.x * 2);
        rect.h = static_cast<int>(renderDelimiter.y * 2);
        rect.x = static_cast<int>(givenPosition.x - renderDelimiter.x);
        rect.y = static_cast<int>(givenPosition.y - renderDelimiter.y);
        SDL_RenderFillRect(renderer, &rect);
    }

private:
    SDL_Color color;
    bool debug_show = false;
};
