#pragma once

#include <SDL2/SDL.h>
#include <cstring>

namespace Input
{
    static bool keys[SDL_NUM_SCANCODES];
    static bool pkeys[SDL_NUM_SCANCODES];

    static int mouse_posx;
    static int mouse_posy;

    Vector2D mouse_pos;

    static Uint32 mouse_buttons = 0;
    static Uint32 prev_mouse_buttons = 0;
    
    void UpdateKeys(SDL_Window* window)
    {
        int mousex, mousey, wW, wH;
        SDL_GetWindowSize(window, &wW, &wH);

        if (!window) return;

        std::memcpy(pkeys, keys, SDL_NUM_SCANCODES);

        const Uint8 *state = SDL_GetKeyboardState(nullptr);
        for (int i = 0; i < SDL_NUM_SCANCODES; i++)
        {
            keys[i] = state[i];
        }

        prev_mouse_buttons = mouse_buttons;
        mouse_buttons = SDL_GetMouseState(&mousex, &mousey);

        mouse_pos = {static_cast<float>(mousex - wW / 2), static_cast<float>(mousey - wH / 2)};
    }
    
    bool GetKey(SDL_Scancode key)
    {
        return keys[key];
    }
    
    bool GetKeyDown(SDL_Scancode key)
    {
        return keys[key] && !pkeys[key];
    }
    
    bool GetKeyUp(SDL_Scancode key)
    {
        return !keys[key] && pkeys[key];
    }

    bool IsLeftClick()
    {
        return mouse_buttons & SDL_BUTTON(SDL_BUTTON_LEFT);
    }

    bool IsLeftClickDown()
    {
        return (mouse_buttons & SDL_BUTTON(SDL_BUTTON_LEFT)) && !(prev_mouse_buttons & SDL_BUTTON(SDL_BUTTON_LEFT));
    }

    bool IsLeftClickUp()
    {
        return !(mouse_buttons & SDL_BUTTON(SDL_BUTTON_LEFT)) && (prev_mouse_buttons & SDL_BUTTON(SDL_BUTTON_LEFT));
    }

    bool IsRightClick()
    {
        return mouse_buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);
    }

    bool IsRightClickDown()
    {
        return (mouse_buttons & SDL_BUTTON(SDL_BUTTON_RIGHT)) && !(prev_mouse_buttons & SDL_BUTTON(SDL_BUTTON_RIGHT));
    }

    bool IsRightClickUp()
    {
        return !(mouse_buttons & SDL_BUTTON(SDL_BUTTON_RIGHT)) && (prev_mouse_buttons & SDL_BUTTON(SDL_BUTTON_RIGHT));
    }

    Vector2D GetMousePosition()
    {
        return mouse_pos;
    }
}