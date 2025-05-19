#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <vector>
#include <scene.h>
#include <utilities_animations.h>

using Grid = std::vector<std::vector<bool>>;

class GameLevel : public Object
{
public:
    GameLevel()
    {
    }

    bool Init(SDL_Renderer *renderer,
              Scene *_mainScene,
              float sizeFactor,
              Vector2D spawn,
              std::string mainPath,
              const std::vector<std::string> &backgroundSpritePaths,
              const std::vector<std::string> &foregroundSpritePaths,
              std::string &rectsPath)
    {
        offset = {0.0f, 0.0f};
        allow_player = true;

        SetFlags(ObjectFlag::Flag_LevelWall);
        collision = false;
        mainScene = _mainScene;
        playerSpawn = spawn;

        InitAnimations(renderer, mainPath, backgroundSpritePaths, foregroundSpritePaths);

        SetSize(sizeFactor);
        InitRects(rectsPath);
        return true;
    }

    void InitAnimations(SDL_Renderer *renderer, std::string mainPath,
                        const std::vector<std::string> &backgroundSpritePaths,
                        const std::vector<std::string> &foregroundSpritePaths)
    {
        if (backgroundSpritePaths.size() != 0)
        {
            backgroundSystem = std::make_unique<AnimationSystem>(renderer, mainPath);
            backgroundSystem->SetEntries({{"main", backgroundSpritePaths, FRAME_DURATION}});

            backgroundSystem->SetAnimation("main");
        }
        if (foregroundSpritePaths.size() != 0)
        {
            foregroundSystem = std::make_unique<AnimationSystem>(renderer, mainPath);
            foregroundSystem->SetEntries({{"main", foregroundSpritePaths, FRAME_DURATION}});

            foregroundSystem->SetAnimation("main");
        }
    }

    void SetEnemies(std::vector<std::shared_ptr<Object>> entities)
    {
        enemies = entities;
        for (auto enemy: enemies)
        {
            enemy->SetParent(this);
            enemy->SetLayerOrder(901);
        }
    }

    std::vector<std::shared_ptr<Object>> GetEnemies()
    {
        return enemies;
    }

    Vector2D SpawnPoint()
    {
        return playerSpawn;
    }

    void InitRects(const std::string &rectsPath)
    {
        if (rectsPath.size() == 0) return;

        auto rects = LoadRects(rectsPath);
        sdlrects.reserve(rects.size());
        for (CoupleRect crect : rects)
        {
            BoxRect brect = crect.brect;
            SDL_Rect sdlrect = crect.sdlrect;

            auto rect_object = mainScene->CreateObject<Wall>();
            rect_object->Init(brect.w * size, brect.h * size);
            rect_object->SetPosition(brect.x * size, brect.y * size);
            rect_object->SetParent(this);

            sdlrects.push_back(sdlrect);
        }

        grid = BuildGrid();
    }

    void SetOffset(const Vector2D &o)
    {
        offset = o;
    }

    void Render(SDL_Renderer *renderer, const Vector2D &givenPosition) override
    {
        if (backgroundSystem == nullptr) return;
        SDL_Texture *tex = backgroundSystem->Next(Time::DeltaTime());

        if (tex)
        {
            int texW, texH;
            SDL_QueryTexture(tex, nullptr, nullptr, &texW, &texH);

            int dstW = static_cast<int>(texW * size);
            int dstH = static_cast<int>(texH * size);

            SDL_FRect dst;
            dst.w = dstW;
            dst.h = dstH;
            dst.x = givenPosition.x - dstW / 2.0f;
            dst.y = givenPosition.y - dstH / 2.0f;

            SDL_RenderCopyF(renderer, tex, nullptr, &dst);
        }

        lastGivenPos = givenPosition;
    }

    void PostRender(SDL_Renderer *renderer)
    {
        if (foregroundSystem == nullptr) return;
        SDL_Texture *tex = foregroundSystem->Next(Time::DeltaTime());

        if (tex)
        {
            int texW, texH;
            SDL_QueryTexture(tex, nullptr, nullptr, &texW, &texH);

            int dstW = static_cast<int>(texW * size);
            int dstH = static_cast<int>(texH * size);

            SDL_FRect dst;
            dst.w = dstW;
            dst.h = dstH;
            dst.x = lastGivenPos.x - dstW / 2.0f;
            dst.y = lastGivenPos.y - dstH / 2.0f;

            SDL_RenderCopyF(renderer, tex, nullptr, &dst);
        }
    }

    void SetSize(float sizeFactor)
    {
        size = sizeFactor;
        collisionDelimiter.x = (100 * size) * 0.5f;
        collisionDelimiter.y = (100 * size) * 0.5f;
        renderDelimiter.x = (100 * size) * 0.5f;
        renderDelimiter.y = (100 * size) * 0.5f;
    }

    Grid BuildGrid()
    {
        int lvlSize = 100;

        Grid grid(lvlSize, std::vector<bool>(lvlSize, false));

        for (auto &r : sdlrects)
        {
            int x0 = std::max(0, r.x);
            int y0 = std::max(0, r.y);
            int x1 = std::min(lvlSize, r.x + r.w);
            int y1 = std::min(lvlSize, r.y + r.h);
            for (int y = y0; y < y1; ++y)
                for (int x = x0; x < x1; ++x)
                    grid[y][x] = true;
        }
        return grid;
    }

    const Grid *GetGrid() const
    {
        return &grid;
    }

    bool IsPlayerAllowed() const
    {
        return allow_player;
    }

    void SetPlayerAllowed(bool is_allowed)
    {
        allow_player = is_allowed;
    }

private:
    std::unique_ptr<AnimationSystem> backgroundSystem, foregroundSystem;
    Vector2D offset, playerSpawn;
    Vector2D lastGivenPos;
    float size = 1.0f;
    Scene *mainScene;
    std::vector<SDL_Rect> sdlrects;
    std::vector<std::shared_ptr<Object>> enemies;
    Grid grid;

    bool allow_player = true;
};
