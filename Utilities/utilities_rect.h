#pragma once

#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <nlohmann/json.hpp> 

struct BoxRect {
    int x;
    int y;
    int w;
    int h;
};

struct CoupleRect
{
    BoxRect brect;
    SDL_Rect sdlrect;

    CoupleRect() : brect({0,0,0,0}), sdlrect({0,0,0,0}){}

    CoupleRect(const BoxRect& br, const SDL_Rect& sr)
    : brect(br), sdlrect(sr)
    {}
};

std::vector<CoupleRect> LoadRects(const std::string& filepath) {
    std::ifstream in{filepath};
    if (!in.is_open()) {
        Debug::Error("Impossible d'ouvrir le fichier JSON : " + filepath);
    }

    nlohmann::json j;
    in >> j;
    if (!j.is_array()) {
        Debug::Error("JSON invalide : attendu un tableau de rectangles");
    }

    std::vector<CoupleRect> crects;
    crects.reserve(j.size());

    for (const auto& item : j) {

        int x0     = item.at("x").get<int>();
        int y0     = item.at("y").get<int>();    
        int width  = item.at("width").get<int>();  
        int height = item.at("height").get<int>();

        int cx = x0 - 50 + width  / 2;
        int cy = y0 - 50 + height / 2;

        int rw = width  / 2;
        int rh = height / 2;

        BoxRect  br{ cx, cy, rw, rh };
        SDL_Rect sr{ x0, y0, width, height };

        crects.emplace_back(br, sr);
    }

    return crects;
}
