#pragma once

#include <camera.h>
#include <vector>
#include <memory>
#include <map>
#include <algorithm>

#include <SDL2/SDL.h>

using SceneObject = std::shared_ptr<Object>;

static constexpr float LEVEL_SIZE_FACTOR = 10.0f;

class Scene
{
public:
    static Scene &Instance()
    {
        static Scene instance;
        return instance;
    }

    Scene(Scene const &) = delete;
    Scene &operator=(Scene const &) = delete;
    Scene(Scene &&) = delete;
    Scene &operator=(Scene &&) = delete;

    void AddObject(SceneObject object)
    {
        objects.push_back(object);
        std::push_heap(objects.begin(), objects.end(), CompareLayer);
    }

    void SetPlayer(SceneObject p)
    {
        player = p;
    }

    std::vector<Object *> GetRawObjectPointers() const
    {
        std::vector<Object *> raw;
        raw.reserve(objects.size());
        for (auto &o : objects)
            raw.push_back(o.get());
        return raw;
    }

    void SetCamera(std::shared_ptr<Camera> cam)
    {
        AddObject(cam);
        camera = cam;
    }

    template <class T>
    std::shared_ptr<T> CreateObject(Vector2D position = {0.0f, 0.0f}, float rotation = 0.0f)
    {
        if (!std::is_base_of_v<Object, T>)
        {
            Debug::Error("Impossible de créer un objet qui n'est pas de la classe Object.");
        }

        auto object = std::make_shared<T>();

        object->SetPosition(position);
        object->SetRotation(rotation);

        AddObject(object);

        return object;
    }

    #include <gamelevel.h>
    std::shared_ptr<GameLevel> CreateLevel(SDL_Renderer *renderer,
        Vector2D spawn,
        Vector2D exitpos,
        std::string mainPath,
        const std::vector<std::string>& backgroundSpritePaths,
        const std::vector<std::string>& foregroundSpritePaths,
        std::string rectsPath)
    {
        auto level = CreateObject<GameLevel>();
        level->Init(renderer,
            this,
            LEVEL_SIZE_FACTOR,
            spawn,
            std::string("Levels/") + mainPath, 
            backgroundSpritePaths,
            foregroundSpritePaths,
            rectsPath);
        level->SetLayerOrder(-1);

        levels[levelCount++] = level;

        auto exit = CreateObject<Exit>();
        exit->Init(EXIT_SIZE, EXIT_SIZE);
        exit->SetParent(level.get());
        exit->SetPosition(exitpos);

        return level;
    }

    void SetGameOver(bool go)
    {
        game_over->SetGameOver(go);
    }

    void SetGameOverObject(std::shared_ptr<GameOver> go)
    {
        game_over = go;
        SetGameOver(false);
    }



    void SetLevel(int index)
    {
        currentLevelIndex = index;
        Vector2D spawnpoint = {0.0f, 0.0f};
        bool player_allowed = true;
        for (auto &[levelindex, level] : levels)
        {
            if (index == levelindex)
            {
                level->SetActive(true);
                player_allowed = level->IsPlayerAllowed();
                spawnpoint = level->SpawnPoint();
            }
            else
            {
                level->SetActive(false);
            }
        }

        player->SetActive(player_allowed);
        if (player_allowed)
        {
            player->SetPosition(spawnpoint);
            player->SetRotation(0.0f);
            player->OnLevelChanged();
        }
    }

    void NextLevel()
    {
        currentLevelIndex++;
        currentLevelIndex %= levels.size();
        SetLevel(currentLevelIndex);
    }

    std::shared_ptr<GameLevel> GetCurrentLevel()
    {
        return levels[currentLevelIndex];
    }
    int GetCurrentLevelIndex()
    {
        return currentLevelIndex;
    }

    void UpdateAll(float deltaTime)
    {
        for (auto &obj : objects)
        {
            if (obj->IsActive())
            {
                obj->Update(deltaTime);
            }
        }
    }

    std::vector<SceneObject> GetObjectsWithFlags(int flags)
    {
        std::vector<SceneObject> result;
        for (SceneObject obj: objects)
        {
            if (obj->HasFlag(flags))
            {
                result.push_back(obj);
            }
        }
        return result;
    }

    void RenderAll(SDL_Renderer *renderer)
    {
        Vector2D camPos = camera->GetWorldPosition();
        Vector2D viewport = camera->GetViewportSize();
        for (auto &obj : objects)
        {
            if (obj->IsActive())
            {
                Vector2D worldPos = obj->GetWorldPosition();

                Vector2D givenPosition = {worldPos.x - camPos.x + viewport.x / 2.f, worldPos.y - camPos.y + viewport.y / 2.f};
                Vector2D delimiter = obj->renderDelimiter;
                if (givenPosition.x - delimiter.x > viewport.x ||
                    givenPosition.x + delimiter.x < 0 ||
                    givenPosition.y - delimiter.y > viewport.y ||
                    givenPosition.y + delimiter.y < 0)
                {
                    obj->SetInvisible(true);
                }
                else
                {
                    obj->SetInvisible(false);
                    obj->Render(renderer, givenPosition);
                }
            }
        }
        for (auto &[levelindex, level] : levels)
        {
            if (level->IsActive())
            {
                level->PostRender(renderer);
            }
        }
    }

    void DestroyObject(SceneObject obj)
    {
        if (obj != nullptr)
        {
            obj->Destroy();
            destroyedObject.push(obj);
        }
    }

    void ClearDestroyedObject()
    {
        while (!destroyedObject.empty())
        {
            auto object = destroyedObject.front();
            object->OnDestroy();

            // destroy the object, bye bye
            object.reset();
            destroyedObject.pop();
        }
    }

    void UpdateLayerOrder()
    {
        std::stable_sort(
            objects.begin(), 
            objects.end(),
            [](const SceneObject &a, const SceneObject &b){
                return a->GetLayerOrder() < b->GetLayerOrder();
            }
        );
    }

    std::shared_ptr<Camera> GetCamera() const
    {
        return camera;
    }

private:
    Scene() = default;
    ~Scene() = default;

    SceneObject player;

    std::vector<SceneObject> objects;
    std::shared_ptr<GameOver> game_over;
    std::shared_ptr<Camera> camera;
    std::map<int, std::shared_ptr<GameLevel>> levels;
    int levelCount = 0, currentLevelIndex = 0;

    std::queue<SceneObject> destroyedObject;

    static bool CompareLayer(const SceneObject &a, const SceneObject &b)
    {
        return a->GetLayerOrder() > b->GetLayerOrder();
    }
};