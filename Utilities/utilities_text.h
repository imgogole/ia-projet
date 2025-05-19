#pragma once

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>

namespace Text
{
    enum Anchor {
        FREE,
        TOP_LEFT,
        TOP_MIDDLE,
        TOP_RIGHT,
        CENTER_LEFT,
        MIDDLE,
        CENTER_RIGHT,
        BOTTOM_LEFT,
        BOTTOM_MIDDLE,
        BOTTOM_RIGHT
    };
    
    /**
     * Affiche du texte positionné selon un point d'ancrage et un pivot.
     *
     * @param renderer       SDL_Renderer*
     * @param font           TTF_Font* (déjà chargé)
     * @param text           std::string, le texte à afficher
     * @param AnchorPointx   int, offset en X depuis l'ancre
     * @param AnchorPointy   int, offset en Y depuis l'ancre
     * @param anchorBase     Anchor, point d'ancrage sur la fenêtre
     * @param pivotPoint     Anchor, point pivot du texte
     * @param color          SDL_Color (par défaut blanc)
     */
    inline void WriteText(SDL_Renderer* renderer,
                          TTF_Font*      font,
                          const std::string& text,
                          int AnchorPointx,
                          int AnchorPointy,
                          Anchor anchorBase,
                          Anchor pivotPoint,
                          SDL_Color color = {255,255,255,255})
    {
        SDL_Surface* surf = TTF_RenderText_Blended(font, text.c_str(), color);
        SDL_Texture* tex  = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_FreeSurface(surf);
    
        int textW, textH;
        SDL_QueryTexture(tex, nullptr, nullptr, &textW, &textH);
    
        int winW, winH;
        SDL_GetRendererOutputSize(renderer, &winW, &winH);
    
        int baseX = 0, baseY = 0;
        switch(anchorBase) {
            case FREE:          baseX = 0;       baseY = 0;       break;
            case TOP_LEFT:      baseX = 0;       baseY = 0;       break;
            case TOP_MIDDLE:    baseX = winW/2;  baseY = 0;       break;
            case TOP_RIGHT:     baseX = winW;    baseY = 0;       break;
            case CENTER_LEFT:   baseX = 0;       baseY = winH/2;  break;
            case MIDDLE:        baseX = winW/2;  baseY = winH/2;  break;
            case CENTER_RIGHT:  baseX = winW;    baseY = winH/2;  break;
            case BOTTOM_LEFT:   baseX = 0;       baseY = winH;    break;
            case BOTTOM_MIDDLE: baseX = winW/2;  baseY = winH;    break;
            case BOTTOM_RIGHT:  baseX = winW;    baseY = winH;    break;
        }
        int anchorX = baseX + AnchorPointx;
        int anchorY = baseY + AnchorPointy;
    
        int pivotOffX = 0, pivotOffY = 0;
        switch(pivotPoint) {
            case FREE:
            case TOP_LEFT:      pivotOffX = 0;       pivotOffY = 0;       break;
            case TOP_MIDDLE:    pivotOffX = textW/2; pivotOffY = 0;       break;
            case TOP_RIGHT:     pivotOffX = textW;   pivotOffY = 0;       break;
            case CENTER_LEFT:   pivotOffX = 0;       pivotOffY = textH/2; break;
            case MIDDLE:        pivotOffX = textW/2; pivotOffY = textH/2; break;
            case CENTER_RIGHT:  pivotOffX = textW;   pivotOffY = textH/2; break;
            case BOTTOM_LEFT:   pivotOffX = 0;       pivotOffY = textH;   break;
            case BOTTOM_MIDDLE: pivotOffX = textW/2; pivotOffY = textH;   break;
            case BOTTOM_RIGHT:  pivotOffX = textW;   pivotOffY = textH;   break;
        }
    
        SDL_Rect dst;
        dst.x = anchorX - pivotOffX;
        dst.y = anchorY - pivotOffY;
        dst.w = textW;
        dst.h = textH;
    
        SDL_RenderCopy(renderer, tex, nullptr, &dst);
        SDL_DestroyTexture(tex);
    }    
}

