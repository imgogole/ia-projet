#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>
#include <string>

class SlideValue : public Object
{
public:
    SlideValue()
    {

    }

    void SetSize(int w, int h)
    {
        renderDelimiter.x = w * .5f;
        renderDelimiter.y = h * .5f;
    }

    void SetColors(SDL_Color fill, SDL_Color empty)
    {
        color_fill = fill;
        color_empty = empty;
    }

    void SetValue(float value)
    {
        slide_value = Math::Clamp01(value);
    }

    void Render(SDL_Renderer *renderer, const Vector2D &givenPosition) override
    {
        int x = givenPosition.x, y = givenPosition.y;
        int sx = renderDelimiter.x * 2, sy = renderDelimiter.y * 2;

        int w1, h, sx1, w2, sx2;

        w1 = x - 0.5f * sx;
        h = y - 0.5f * sy;
        sx1 = sx * slide_value;

        w2 = w1 + sx1;
        sx2 = sx * (1 - slide_value);

        if (SLIDER_OUTSCALE > 0)
        {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_Rect rect0;
            rect0.w = static_cast<int>(sx) + SLIDER_OUTSCALE * 2;
            rect0.h = static_cast<int>(sy) + SLIDER_OUTSCALE * 2;
            rect0.x = static_cast<int>(w1) - SLIDER_OUTSCALE;
            rect0.y = static_cast<int>(h) - SLIDER_OUTSCALE;
            SDL_RenderFillRect(renderer, &rect0);
        }

        // dessiner le rectangle côté rempli

        SDL_SetRenderDrawColor(renderer, color_fill.r, color_fill.g, color_fill.b, color_fill.a);
        SDL_Rect rect1;
        rect1.w = static_cast<int>(sx1);
        rect1.h = static_cast<int>(sy);
        rect1.x = static_cast<int>(w1);
        rect1.y = static_cast<int>(h);
        SDL_RenderFillRect(renderer, &rect1);

        // dessiner le rectangle côté vide

        SDL_SetRenderDrawColor(renderer, color_empty.r, color_empty.g, color_empty.b, color_empty.a);
        SDL_Rect rect2;
        rect2.w = static_cast<int>(sx2);
        rect2.h = static_cast<int>(sy);
        rect2.x = static_cast<int>(w2);
        rect2.y = static_cast<int>(h);
        SDL_RenderFillRect(renderer, &rect2);
    }

private:
    SDL_Color color_fill, color_empty;
    float slide_value = 0.0f;
};
