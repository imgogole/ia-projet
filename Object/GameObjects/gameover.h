#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>
#include <string>

class GameOver : public Object
{
public:
    GameOver()
    {
        SetPosition(0.0f, 0.0f);
        SetRotation(.0f);
    }

    ~GameOver()
    {
        if (game_over_img)
            SDL_DestroyTexture(game_over_img);
    }

    void Init(SDL_Renderer *renderer)
    {
        collision = false;

        SDL_Surface *surf = IMG_Load("Assets/game_over.png");
        if (!surf)
        {
            SDL_Log("Failed to load shadow: %s", IMG_GetError());
            return;
        }

        game_over_img = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_FreeSurface(surf);

        if (!game_over_img)
        {
            SDL_Log("Failed to create texture: %s", SDL_GetError());
            return;
        }

        SetLayerOrder(11000);
        SDL_SetTextureAlphaMod(game_over_img, alpha);

        SetGameOver(false);
    }

    void SetGameOver(bool go)
    {
        game_over = go;
        SetActive(go);
        alpha = 0;
    }

    void Update(float deltaTime) override
    {
        if (game_over_img && game_over && alpha < 250)
        {
            alpha = Math::Lerp(alpha, 255, deltaTime * 5.0f);

            SDL_SetTextureAlphaMod(game_over_img, alpha);
        }
    }

    void Render(SDL_Renderer *renderer, const Vector2D &givenPosition) override
    {
        int w, h;
        SDL_GetRendererOutputSize(renderer, &w, &h);

        SDL_Rect rect;
        rect.w = w;
        rect.h = h;
        rect.x = 0;
        rect.y = 0;
        
        SDL_RenderCopy(renderer, game_over_img, nullptr, &rect);
    }

private:
bool game_over;
Uint8 alpha;
    SDL_Texture *game_over_img;
};