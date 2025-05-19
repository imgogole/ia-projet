#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>
#include <string>
#include <algorithm>
#include <utilities_animations.h>

class Player : public Entity
{
public:
    Player()
    {
        SetPosition(0.0f, 0.0f);
        SetFlags(ObjectFlag::Flag_Player);
        renderDelimiter = {50.0f, 50.0f};
        collisionDelimiter = renderDelimiter * 0.5f;
        delimiterAffectedByRotation = false;
        velocity = {0.0f, 0.0f};
        walking = false;
        frameTimer = 0.0f;
        collision = true;
        isDead = false;
        autoLock = false;
        attackSpeed = PLAYER_ATTACK_SPEED;
        did_death_animation = false;
    }

    ~Player()
    {
    }

    void InitPlayer(SDL_Renderer *renderer)
    {
        SetMaxHP(PLAYER_MAX_HP);
        InnerInit(renderer, 1.0f);

        cinematic_system = Scene::Instance().CreateObject<EndVideo>();
        cinematic_system->Init(renderer);
        cinematic_system->SetParent(this);
        cinematic_system->SetActive(true);
        cinematic_system->SetLayerOrder(9999);

        animationSystem = std::make_unique<AnimationSystem>(renderer, "MC/");
        animationSystem->SetEntries({{"idle",
                                      {"mc_idle.png"},
                                      FRAME_DURATION},

                                     {"walk",
                                      {"mc_idle.png",
                                       "mc_walk_left_1.png",
                                       "mc_walk_left_2.png",
                                       "mc_walk_right_1.png",
                                       "mc_walk_right_2.png"},
                                      FRAME_DURATION,
                                      {0, 1, 2, 1, 0, 3, 4, 3}},

                                     {"attack",
                                      {"mc_after_attack_0.png",
                                       "mc_after_attack_1.png",
                                       "mc_after_attack_2.png",
                                       "mc_attack_1.png",
                                       "mc_attack_2.png",
                                       "mc_idle.png"},
                                      ATTACK_FRAME_DURATION,
                                      {2, 3, 4, 0, 1, 2, 5}},

                                     {"sprint",
                                      {"mc_idle.png",
                                       "mc_walk_left_1.png",
                                       "mc_walk_left_2.png",
                                       "mc_walk_right_1.png",
                                       "mc_walk_right_2.png"},
                                      FRAME_DURATION * PLAYER_SPEED_SPRINT_MUL,
                                      {0, 1, 2, 1, 0, 3, 4, 3}},

                                     {"dead",
                                      {"mc_death_1.png",
                                       "mc_death_2.png",
                                       "mc_death_3.png",
                                       "mc_death_4.png",
                                       "mc_death_5.png",
                                       "mc_death_6.png",
                                       "mc_death_7.png"},
                                      FRAME_DURATION}});

        attack_speed_slide = Scene::Instance().CreateObject<SlideValue>();
        attack_speed_slide->SetParent(this);
        attack_speed_slide->SetPosition(0, 80);
        attack_speed_slide->SetColors({230, 225, 94, 255}, {50, 50, 50, 255});
        attack_speed_slide->SetSize(50, 5);
        attack_speed_slide->SetLayerOrder(800);
        attack_speed_slide->SetActive(false);
    }

    void OnLevelChanged() override
    {
        InitEnemies(Scene::Instance().GetCurrentLevel()->GetEnemies());
        SetHP(maxHP);
        if (Scene::Instance().GetCurrentLevelIndex() == BOSS_LEVEL)
        {
            SoundManager::Instance().StopAll();
            SoundManager::Instance().PlaySound("boss_ost", true, 0.5f);
            cinematic_system->StartVideo("boss");
        }
        did_death_animation = false;
    }

    bool AutoLock() const
    {
        return autoLock;
    }

    void InitEnemies(std::vector<std::shared_ptr<Object>> enemies_)
    {
        enemies = enemies_;
    }

    Object *GetNearestEnemy(Vector2D pos)
    {
        if (enemies.size() == 0)
            return nullptr;
        Object *result = nullptr;
        float norm = 10000;
        for (auto entity : enemies)
        {
            Object *obj = entity.get();
            Entity *enemy = dynamic_cast<Entity *>(obj);
            if (enemy)
            {
                if (enemy->IsDead())
                    continue;
                float thisnorm = (enemy->GetWorldPosition() - pos).norm1();
                if (thisnorm < norm)
                {
                    norm = thisnorm;
                    result = enemy;
                }
            }
        }
        return result;
    }

    void Update(float deltaTime) override
    {
        velocity = {0.0f, 0.0f};

        bool cinematic = cinematic_system && !cinematic_system->End();

        if (Input::GetKeyDown(SDL_SCANCODE_L))
        {
            autoLock = !autoLock;
        }

        if (isDead)
        {
            animationSystem->SetAnimation("dead", false);
            attack_speed_slide->SetActive(false);
            return;
        }

        Vector2D worldPos = GetWorldPosition();
        float rotation = GetRotation();
        Vector2D mousePos = Input::GetMousePosition() - worldPos;

        if (autoLock)
        {
            Object *nearestEnemy = GetNearestEnemy(worldPos);
            if (nearestEnemy != nullptr)
            {
                SetRotation((nearestEnemy->GetWorldPosition() - worldPos).to_angle() * RAD2DEG);
            }
            else
            {
                autoLock = false;
            }
        }
        else
        {
            SetRotation(mousePos.to_angle() * RAD2DEG);
        }

        bool attacking = IsAttacking();
        bool too_near = mousePos.sqr_norm() < SQR_MIN_DISTANCE_TO_MOVE;
        float sprint_mul = 1.0f;
        bool IsSprinting = Input::GetKey(SDL_SCANCODE_LSHIFT);
        if (IsSprinting)
            sprint_mul = PLAYER_SPEED_SPRINT_MUL;
        float speed = PLAYER_SPEED * sprint_mul;

        if (!cinematic)
        {
            if (Input::GetKey(SDL_SCANCODE_W))
            {
                velocity += {0.0f, -1.0f};
            }
            if (Input::GetKey(SDL_SCANCODE_S))
            {
                velocity += {0.0f, 1.0f};
            }
            if (Input::GetKey(SDL_SCANCODE_A))
            {
                velocity += {-1.0f, 0.0f};
            }
            if (Input::GetKey(SDL_SCANCODE_D))
            {
                velocity += {1.0f, 0.0f};
            }
        }

        if (INCLUDE_ROTATION_VELOCITY)
        {
            if (too_near)
            {
                velocity = {0.0f, 0.0f};
            }
            else
            {
                float angle = mousePos.to_angle() + PI_OVER_TWO;
                velocity = velocity.rotate(angle).normalize();
            }
        }

        velocity *= speed;

        if (attacking)
            velocity *= SLOW_WHEN_ATTACK_FACTOR;

        Translate(velocity.x * deltaTime, velocity.y * deltaTime);

        // Attack

        if (attacking)
        {
            attackRemaining -= deltaTime;

            attack_speed_slide->SetValue(attackRemaining / attackSpeed + 0.001f);

            if (attackRemaining <= 0)
            {
                attack_speed_slide->SetActive(false);
            }
        }
        else
        {
            attack_speed_slide->SetActive(false);

            if (!cinematic && Input::IsLeftClickDown())
            {
                attackRemaining = attackSpeed;
                attack_speed_slide->SetActive(true);

                SoundManager::Instance().PlaySound("swing_sword");

                auto raycast = Collision::Raycast(
                    worldPos,
                    rotation,
                    PLAYER_RANGE,
                    Scene::Instance().GetRawObjectPointers(),
                    Flag_Wall,
                    Flag_Player | Flag_Dead);

                if (raycast.size() > 0)
                {


                    if (Entity *enemy = dynamic_cast<Entity *>(raycast[0]))
                    {
                        if (!enemy->IsDead())
                        {
                            SoundManager::Instance().PlaySound("hit");
                            enemy->Damage(PLAYER_DAMAGE);
                        }
                    }
                }

                if (!did_death_animation && enemies.size() != 0 && AllEnemiesDead() && Scene::Instance().GetCurrentLevelIndex() != BOSS_LEVEL)
                {
                    cinematic_system->StartVideo("end_kill");
                    did_death_animation = true;
                }
            }
        }

        // contrôler les animations
        if (attacking)
        {
            animationSystem->SetAnimation("attack", false);
        }
        else
        {
            if (velocity.zero())
            {
                animationSystem->SetAnimation("idle", false);
            }
            else if (IsSprinting)
            {
                animationSystem->SetAnimation("sprint", true);
            }
            else
            {
                animationSystem->SetAnimation("walk", true);
            }
        }
    }

    void OnCollisionEnter(Object *collision)
    {
        if (collision->HasFlag(ObjectFlag::Flag_Exit))
        {
            if (Scene::Instance().GetCurrentLevelIndex() != BOSS_LEVEL)
            {
                if (AllEnemiesDead())
                {
                    Scene::Instance().NextLevel();
                }
                else
                {
                    Debug::Log("Attempted to change level when ennemies are not died yet.");
                }
            }
            else
            {
                if (AllEnemiesDead())
                {
                    Application::Quit();
                }
                else
                {
                    Debug::Log("Attempted to end game when the boss is still alive.");
                }
            }
        }
    }

    bool AllEnemiesDead()
    {
        bool all_enemies_dead = true;
        for (auto entity : enemies)
        {
            if (Entity *enemy = dynamic_cast<Entity *>(entity.get()))
            {
                if (!enemy->IsDead())
                {
                    return false;
                }
            }
        }
        return true;
    }

    void OnCollisionStay(Object *collision)
    {
        if (collision->HasFlag(ObjectFlag::Flag_Wall))
        {
            Vector2D axis;
            float overlap;
            if (!Collision::ComputeMTV(this, collision, axis, overlap))
                return;

            Translate(-axis.x * overlap, -axis.y * overlap);

            Vector2D tangent{-axis.y, axis.x};
            float tlen = tangent.norm();
            if (tlen > 0.0f)
                tangent = tangent * (1.0f / tlen);

            float dot = velocity.x * tangent.x + velocity.y * tangent.y;
            Vector2D slideVel{tangent.x * dot, tangent.y * dot};

            slideVel *= WALL_FRICTION;

            float dt = Time::DeltaTime();
            Translate(slideVel.x * dt, slideVel.y * dt);
        }
    }

    void Render(SDL_Renderer *renderer, const Vector2D &givenPosition) override
    {
        SDL_Texture *tex = animationSystem->Next(Time::DeltaTime());

        int texW, texH;
        SDL_QueryTexture(tex, nullptr, nullptr, &texW, &texH);

        int dstW = static_cast<int>(texW * TEXTURE_SCALE);
        int dstH = static_cast<int>(texH * TEXTURE_SCALE);

        SDL_Rect dst;
        dst.w = dstW;
        dst.h = dstH;
        dst.x = static_cast<int>(givenPosition.x - dstW / 2.0f);
        dst.y = static_cast<int>(givenPosition.y - dstH / 2.0f);

        SDL_Point center{dstW / 2, dstH / 2};

        SDL_RenderCopyEx(renderer,
                         tex,
                         nullptr,
                         &dst,
                         GetRotation() + 90,
                         &center,
                         SDL_FLIP_NONE);
    }

private:
    std::shared_ptr<SlideValue> attack_speed_slide;
    std::shared_ptr<EndVideo> cinematic_system;
    std::vector<std::shared_ptr<Object>> enemies;
    bool autoLock;
    bool did_death_animation = false;
};
