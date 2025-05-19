#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>
#include <string>

class TriggerEnemy : public Object
{
public:
    TriggerEnemy()
    {
        SetPosition(0.0f, 0.0f);
    }

    ~TriggerEnemy()
    {
        if (trigger_img)
            SDL_DestroyTexture(trigger_img);
    }

    void Init(SDL_Renderer *renderer, Object* _parent, float size = 25.0f)
    {
        collision = false;
        SetParent(_parent);
        SetRotation(.0f);
        renderDelimiter = { size, size };

        SDL_Surface *surf = IMG_Load("Assets/Hey.png");
        if (!surf)
        {
            SDL_Log("Failed to load shadow: %s", IMG_GetError());
            return;
        }

        trigger_img = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_FreeSurface(surf);

        if (!trigger_img)
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
        SDL_RenderCopy(_renderer, trigger_img, nullptr, &rect);
    }

private:
    SDL_Texture *trigger_img;
};