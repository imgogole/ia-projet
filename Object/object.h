#pragma once

#include <vector>
#include <memory>
#include <SDL2/SDL.h>

enum ObjectFlag
{
    None = 0,
    Flag_Player = 1 << 0,
    Flag_Enemy = 1 << 1,
    Flag_Wall = 1 << 2,
    Flag_LevelWall = 1 << 3,
    Flag_Exit = 1 << 4,
    Flag_Dead = 1 << 5,
};

class Object
{
public:
    Object()
    {
        SetActive(true);
        SetPosition(0, 0);
        SetRotation(.0f);
        SetLayerOrder(0);

        movedThisFrame = false;
        Start();
    }
    virtual ~Object() = default;

    virtual void Start()
    {
    }
    virtual void Update(float deltaTime) {}
    virtual void Render(SDL_Renderer *renderer, const Vector2D &givenPosition) {}

    virtual void OnCollisionEnter(Object *collision) {}
    virtual void OnCollisionStay(Object *collision) {}
    virtual void OnCollisionExit(Object *collision) {}

    virtual void OnLevelChanged() { }

    void SetPosition(float x, float y)
    {
        position.x = x;
        position.y = y;

        movedThisFrame = true;
    }

    void SetPosition(const Vector2D &v)
    {
        position = v;

        movedThisFrame = true;
    }

    void Translate(float x, float y)
    {
        SetPosition(position.x + x, position.y + y);
    }

    inline Vector2D GetLocalPosition() const
    {
        return position;
    }

    Vector2D GetWorldPosition() const
    {
        Vector2D v = {position.x, position.y};
        Object *p = parent;
        while (p)
        {
            v.x += p->position.x;
            v.y += p->position.y;

            p = p->parent;
        }
        return v;
    }

    void SetLayerOrder(int order)
    {
        layerOrder = order;
    }

    void SetRotation(float r)
    {
        rotation = r;
    }

    void SetInvisible(bool isInvisible)
    {
        invisible = isInvisible;
    }

    void SetParent(Object *ptr_parent)
    {
        parent = ptr_parent;
        parent->children.push_back(this);
    }

    bool IsSelfActive() const
    {
        return active;
    }

    bool IsActive() const
    {
        return IsSelfActive() && (parent == nullptr || parent->IsSelfActive());
    }

    int GetLayerOrder() const
    {
        return layerOrder;
    }

    float GetRotation() const
    {
        return rotation;
    }

    void SetActive(bool a)
    {
        active = a;
        for (auto child : children)
        {
            child->SetActive(a);
        }
    }

    void SetFlags(int new_flags)
    {
        flags = new_flags;
    }

    void AddFlags(int new_flags)
    {
        flags |= new_flags;
    }

    bool HasFlag(int f) const
    {
        return (flags & f) != 0;
    }

    int GetFlags() const
    {
        return flags;
    }

    void RemoveFlag(int f)
    {
        flags &= ~f;
    }

    void Destroy()
    {
        destroyed = true;
    }

    Vector2D renderDelimiter;
    Vector2D collisionDelimiter;
    bool collision = false;
    bool delimiterAffectedByRotation = false;

    bool invisible;

    Object *parent = nullptr;
    std::vector<Object *> children;

    bool operator==(std::nullptr_t) const { return destroyed; }
    bool operator!=(std::nullptr_t) const { return !destroyed; }
    friend bool operator==(std::nullptr_t, const Object &o) { return o == nullptr; }
    friend bool operator!=(std::nullptr_t, const Object &o) { return o != nullptr; }

    explicit operator bool() const { return !destroyed; }

    virtual void OnDestroy() {}

private:
    Vector2D position;
    int layerOrder;
    bool active;
    bool movedThisFrame;
    float rotation;
    int flags;
    bool destroyed = false;
};