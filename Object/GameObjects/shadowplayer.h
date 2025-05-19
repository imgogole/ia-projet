#pragma once

#include "object.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>
#include <string>

class ShadowPlayer : public Object
{
public:
    ShadowPlayer()
    {
        SetPosition(0.0f, 0.0f);
    }

    ~ShadowPlayer()
    {
        if (shadow_img)
            SDL_DestroyTexture(shadow_img);
    }

    void Init(SDL_Renderer *renderer, Object* _parent, float sizeMultiplier)
    {
        collision = false;
        SetParent(_parent);
        SetPosition(0, 0);
        SetRotation(.0f);
        renderDelimiter = (_parent->renderDelimiter) * sizeMultiplier;

        SDL_Surface *surf = IMG_Load("Assets/Shadow.png");
        if (!surf)
        {
            SDL_Log("Failed to load shadow: %s", IMG_GetError());
            return;
        }

        shadow_img = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_FreeSurface(surf);

        if (!shadow_img)
        {
            SDL_Log("Failed to create texture: %s", SDL_GetError());
            return;
        }

        SetLayerOrder(0);
    }

    void Render(SDL_Renderer *_renderer, const Vector2D &givenPosition) override
    {
        float offset_x = 0.0f, offset_y = 0.0f;
        SDL_Rect rect;
        rect.w = static_cast<int>(renderDelimiter.x * 2.0f);
        rect.h = static_cast<int>(renderDelimiter.y * 2.0f);
        rect.x = static_cast<int>(givenPosition.x - renderDelimiter.x + offset_x);
        rect.y = static_cast<int>(givenPosition.y - renderDelimiter.y + offset_y);
        SDL_RenderCopy(_renderer, shadow_img, nullptr, &rect);
    }

private:
    SDL_Renderer *renderer;
    SDL_Texture *shadow_img;
};