#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>
#include <string>
#include <algorithm>
#include <utilities_animations.h>

class Entity : public Object
{
public:
    void InnerInit(SDL_Renderer *renderer, float shadowSize = 1.0f)
    {
        auto shadow = Scene::Instance().CreateObject<ShadowPlayer>();
        shadow->Init(renderer, this, shadowSize);

        hp_slide = Scene::Instance().CreateObject<SlideValue>();
        hp_slide->SetParent(this);
        hp_slide->SetPosition(0, -80);
        hp_slide->SetColors({123, 240, 91, 255}, {50, 50, 50, 255});
        hp_slide->SetSize(65, 8);
        hp_slide->SetLayerOrder(800);
        SetHP(maxHP);

    }

    void Damage(float amount)
    {
        SetHP(currentHP - amount);
    }

    void SetHP(float hp)
    {
        currentHP = Math::Clamp(hp, 0.0f, maxHP);

        if (currentHP > 0.0f)
        {
            isDead = false;
            hp_slide->SetActive(true);
            hp_slide->SetValue(currentHP/maxHP);
        }
        else
        {
            hp_slide->SetActive(false);
            isDead = true;
            AddFlags(Flag_Dead);

            SoundManager::Instance().PlaySound("death");

            SetLayerOrder(0);
            Scene::Instance().UpdateLayerOrder();

            OnDeath();
        }
    }

    virtual void OnDeath() { }

    bool IsDead() const 
    {
        return isDead;
    }

    bool IsAttacking() const
    {
        return attackRemaining > 0.0f;
    }

    void SetMaxHP(float newMaxHP)
    {
        maxHP = newMaxHP;
    }

protected:
    float maxHP       = 100.0f;
    float currentHP   = maxHP;
    bool  isDead      = false;

    std::unique_ptr<AnimationSystem> animationSystem;
    std::shared_ptr<SlideValue> hp_slide;

    float attackSpeed     = 0.35f;
    float attackRemaining = 0.0f;
    float attackCast      = 0.5f;

    Vector2D velocity{0,0};
    bool     walking{false};
    float    frameTimer{0};
};
