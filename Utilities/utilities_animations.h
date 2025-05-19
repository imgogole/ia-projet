#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <vector>
#include <memory>
#include <initializer_list>
#include <cmath>

struct AnimationEntry
{
    std::string id, mainPath;
    float frameDuration;
    std::vector<SDL_Texture *> frames;
    std::vector<int> pattern;
    size_t patternPos = 0;
    float timer = 0.0f;
    float entryTime;

    AnimationEntry(SDL_Renderer *renderer,
                   std::string id_,
                   std::string mainPath_,
                   const std::vector<std::string> &paths,
                   float frameDuration_,
                   const std::vector<int> &pattern_ = {})
        : id(std::move(id_)), mainPath(std::move(mainPath_)), frameDuration(frameDuration_)
    {
        frames.reserve(paths.size());
        for (auto const &p : paths)
        {
            SDL_Texture *tex = IMG_LoadTexture(renderer, (std::string("Assets/") + mainPath + p).c_str());
            if (!tex)
            {
                Debug::Error("IMG_LoadTexture failed for " + p + ": " + IMG_GetError());
            }
            frames.push_back(tex);
        }
        if (pattern_.empty())
        {
            // pattern séquentiel 0,1,...,N-1
            pattern.resize(frames.size());
            for (size_t i = 0; i < frames.size(); ++i)
                pattern[i] = static_cast<int>(i);
        }
        else
        {
            pattern = pattern_;
        }

        entryTime = frameDuration * frames.size();
    }

    ~AnimationEntry()
    {
        for (auto *tex : frames)
        {
            if (tex)
                SDL_DestroyTexture(tex);
        }
    }

    void Reset()
    {
        patternPos = 0;
        timer = 0.0f;
    }

    float EntryTime() const
    {
        return entryTime;
    }

    SDL_Texture *Next(float deltaTime, bool loop)
    {
        if (frames.empty() || pattern.empty())
            return nullptr;
        timer += deltaTime;
        if (timer >= frameDuration)
        {
            float overshoot = std::fmod(timer, frameDuration);
            size_t steps = static_cast<size_t>((timer - overshoot) / frameDuration);
            timer = overshoot;
            patternPos += steps;
            if (patternPos >= pattern.size())
            {
                if (loop)
                    patternPos %= pattern.size();
                else
                    patternPos = pattern.size() - 1;
            }
        }
        int idx = pattern[patternPos];
        if (idx < 0 || idx >= static_cast<int>(frames.size()))
        {
            Debug::Error("AnimationEntry: frame index out of range for '" + id + "'");
            return frames.front();
        }
        return frames[idx];
    }

    bool Finished() const
    {
        return patternPos == pattern.size() - 1;
    }
};

/**
 * Simplifie la création de listes d’entries
 */
struct AnimationEntryTmp
{
    std::string id;
    std::vector<std::string> paths;
    float frameDuration;
    std::vector<int> pattern;

    // constructeur sans pattern explicite
    AnimationEntryTmp(std::string id_,
                      std::vector<std::string> paths_,
                      float duration_)
        : id(std::move(id_)), paths(std::move(paths_)), frameDuration(duration_), pattern{}
    {
    }

    // constructeur avec pattern
    AnimationEntryTmp(std::string id_,
                      std::vector<std::string> paths_,
                      float duration_,
                      std::vector<int> pattern_)
        : id(std::move(id_)), paths(std::move(paths_)), frameDuration(duration_), pattern(std::move(pattern_))
    {
    }
};

/**
 * Le système d’animations, avec surcharge SetEntries()
 */
class AnimationSystem
{
public:
    AnimationSystem(SDL_Renderer *renderer, std::string mainpath)
        : renderer_(renderer), currentEntry_(nullptr), loop_(true), mainPath(mainpath)
    {
    }

    // ancienne méthode conservée si besoin
    void AddAnimation(const std::string &id,
                      const std::vector<std::string> &paths,
                      float frameDuration,
                      const std::vector<int> &pattern = {})
    {
        entries_.emplace_back(
            std::make_unique<AnimationEntry>(renderer_, id, mainPath, paths, frameDuration, pattern));
    }

    // *** nouvelle surcharge ***
    void SetEntries(std::initializer_list<AnimationEntryTmp> list)
    {
        entries_.clear();
        currentEntry_ = nullptr;
        for (auto const &tmp : list)
        {
            entries_.emplace_back(
                std::make_unique<AnimationEntry>(
                    renderer_,
                    tmp.id,
                    mainPath,
                    tmp.paths,
                    tmp.frameDuration,
                    tmp.pattern));
        }
    }

    bool SetAnimation(const std::string &id, bool loop = true)
    {
        if (currentEntry_ && currentEntry_->id == id)
        {
            loop_ = loop;
            return true;
        }
        for (auto &e : entries_)
        {
            if (e->id == id)
            {
                currentEntry_ = e.get();
                currentEntry_->Reset();
                loop_ = loop;
                return true;
            }
        }
        Debug::Error("AnimationSystem: animation '" + id + "' not found");
        return false;
    }

    SDL_Texture *Next(float deltaTime)
    {
        if (!currentEntry_)
            return nullptr;
        return currentEntry_->Next(deltaTime, loop_);
    }

    void ResetAnimation()
    {
        if (!currentEntry_)
            return;
        return currentEntry_->Reset();
    }

    bool Finished() const
    {
        if (!currentEntry_)
            return false;
        return currentEntry_->Finished();
    }

    float EntryTime() const
    {
        if (!currentEntry_) return 0.0f;
        return currentEntry_->EntryTime();
    }

    // accès à l’ID ou à la texture courante si besoin
    const std::string &CurrentId() const
    {
        static std::string empty;
        return currentEntry_ ? currentEntry_->id : empty;
    }
    SDL_Texture *Current() const
    {
        return currentEntry_ ? currentEntry_->frames[currentEntry_->pattern[currentEntry_->patternPos]] : nullptr;
    }

private:
    SDL_Renderer *renderer_;
    std::vector<std::unique_ptr<AnimationEntry>> entries_;
    AnimationEntry *currentEntry_;
    bool loop_;
    std::string mainPath;
};
