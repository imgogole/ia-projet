#pragma once

#include <vector>
#include <chrono>
#include <future>
#include <memory>

#include <utilities_astar.h>

using Grid = std::vector<std::vector<bool>>;

class Enemy : public Entity
{
public:
    Enemy()
    {
        SetFlags(ObjectFlag::Flag_Enemy);
        collision = true;
        delimiterAffectedByRotation = false;

        renderDelimiter = {50.0f, 50.0f};
        collisionDelimiter = renderDelimiter;

        color_ = {255, 0, 0, 255};
        debug_show_ = false;

        computingPath_ = false;
        pathIndex_ = 0;
        grid_ = nullptr;

        is_chasing = false;
        target_was_player = false;
    }

    void InitEnemy(SDL_Renderer *renderer, std::vector<Vector2D> idlepoints, bool isBoss = false)
    {
        is_boss = isBoss;
        SetMaxHP(isBoss ? BOSS_MAX_HP : ENEMY_MAX_HP);

        idle_points = idlepoints;
        idle_points.push_back(GetWorldPosition()); // assurer qu'il y a toujours au moins un point

        InnerInit(renderer, 0.75f);

        if (!is_boss)
        {
            animationSystem = std::make_unique<AnimationSystem>(renderer, "ENEMY1/");
            animationSystem->SetEntries({{"idle",
                                          {"enemy1_idle.png"},
                                          FRAME_DURATION},

                                         {"walk",
                                          {
                                              "enemy1_idle.png",
                                              "enemy1_idle_step_left_1.png",
                                              "enemy1_idle_step_left_2.png",
                                              "enemy1_idle_step_right_1.png",
                                              "enemy1_idle_step_right_2.png",
                                          },
                                          FRAME_DURATION,
                                          {0, 1, 2, 1, 0, 3, 4, 3}},

                                         {"attack",
                                          {"enemy1_attack_0.png",
                                           "enemy1_attack_1.png",
                                           "enemy1_attack_2.png",
                                           "enemy1_after_attack_0.png",
                                           "enemy1_after_attack_1.png",
                                           "enemy1_after_attack_2.png"},
                                          ATTACK_FRAME_DURATION},

                                         {"chase",
                                          {
                                              "enemy1_ready.png",
                                              "enemy1_step_left_1.png",
                                              "enemy1_step_left_2.png",
                                              "enemy1_step_right_1.png",
                                              "enemy1_step_right_2.png",
                                          },
                                          FRAME_DURATION * PLAYER_SPEED_SPRINT_MUL,
                                          {0, 1, 2, 1, 0, 3, 4, 3}},

                                         {"dead",
                                          {
                                              "enemy1_death1_1.png",
                                              "enemy1_death1_2.png",
                                              "enemy1_death1_3.png",
                                              "enemy1_death1_4.png",
                                              "enemy1_death1_5.png",
                                              "enemy1_death1_6.png",
                                          },
                                          FRAME_DURATION}});
        }
        else
        {
            animationSystem = std::make_unique<AnimationSystem>(renderer, "BOSS/");
            animationSystem->SetEntries({{"idle",
                                          {"boss_idle.png"},
                                          FRAME_DURATION},

                                         {"walk",
                                          {
                                              "boss_idle.png",
                                              "boss_step_left_1.png",
                                              "boss_step_left_2.png",
                                              "boss_step_right_1.png",
                                              "boss_step_right_2.png",
                                          },
                                          FRAME_DURATION,
                                          {0, 1, 2, 1, 0, 3, 4, 3}},

                                         {"attack",
                                          {"boss_attack_0.png",
                                           "boss_attack_1.png",
                                           "boss_attack_2.png",
                                           "boss_after_attack_0.png",
                                           "boss_after_attack_1.png",
                                           "boss_after_attack_2.png"},
                                          ATTACK_FRAME_DURATION},

                                         {"chase",
                                          {
                                              "boss_idle.png",
                                              "boss_step_left_1.png",
                                              "boss_step_left_2.png",
                                              "boss_step_right_1.png",
                                              "boss_step_right_2.png",
                                          },
                                          FRAME_DURATION * PLAYER_SPEED_SPRINT_MUL,
                                          {0, 1, 2, 1, 0, 3, 4, 3}},

                                         {"dead",
                                          {
                                              "boss_death1_1.png",
                                              "boss_death1_2.png",
                                              "boss_death1_3.png",
                                              "boss_death1_4.png",
                                              "boss_death1_5.png",
                                              "boss_death1_6.png",
                                          },
                                          FRAME_DURATION}});
        }

        attackSpeed = is_boss ? BOSS_ATTACK_SPEED : ENEMY_ATTACK_SPEED;

        trigger = Scene::Instance().CreateObject<TriggerEnemy>();
        trigger->Init(renderer, this);
        trigger->SetPosition({40, -110});
    }

    void SetPlayer(const std::shared_ptr<Entity> &p)
    {
        player_ = p;
    }
    void SetGrid(const Grid *grid)
    {
        grid_ = grid;
    }
    void Update(float dt) override
    {
        trigger->SetActive(!IsDead() && is_chasing);

        if (isDead)
        {
            animationSystem->SetAnimation("dead", false);
            return;
        }

        Vector2D playerPos = player_->GetWorldPosition();
        Vector2D myPos = GetWorldPosition();

        bool attacking = IsAttacking();

        bool can_chase_player = is_chasing && player_ && !player_->IsDead();
        Vector2D me_to_player_v = (myPos - playerPos);
        float range_to_player = me_to_player_v.sqr_norm();

        if (grid_)
        {
            if (computingPath_)
            {
                if (pathFuture_.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
                {
                    path_ = pathFuture_.get();
                    pathIndex_ = 0;
                    computingPath_ = false;
                }
            }
            else
            {

                if (pathIndex_ >= path_.size() || can_chase_player && !target_was_player)
                {
                    Vector2D target_pos;
                    if (can_chase_player && !target_was_player)
                    {
                        target_pos = playerPos;
                        target_was_player = true;
                    }
                    else
                    {
                        target_pos = can_chase_player ? playerPos : Random::Choose(idle_points);
                    }
                    requestPathTo(target_pos);
                }
                else
                {
                    Vector2D target = path_[pathIndex_];
                    Vector2D pos = GetWorldPosition();
                    Vector2D delta = target - pos;
                    float dist = delta.sqr_norm();

                    constexpr float ARRIVE_THRESH = (LEVEL_SIZE_FACTOR * 0.5f) * (LEVEL_SIZE_FACTOR * 0.5f);
                    if (dist < ARRIVE_THRESH)
                    {
                        ++pathIndex_;
                    }
                    else
                    {
                        if (is_boss)
                        {
                            speed_ = can_chase_player ? BOSS_SPEED_CHASE : BOSS_SPEED_IDLE;
                        }
                        else
                        {
                            speed_ = can_chase_player ? ENEMY_SPEED_CHASE : ENEMY_SPEED_IDLE;
                        }

                        if (attacking)
                            speed_ *= SLOW_WHEN_ATTACK_FACTOR;

                        Vector2D desiredDir = delta.normalize();
                        Vector2D desiredVel = desiredDir * speed_;

                        float t = std::clamp(smoothFactor_ * dt, 0.f, 1.f);
                        velocity.x = velocity.x * (1 - t) + desiredVel.x * t;
                        velocity.y = velocity.y * (1 - t) + desiredVel.y * t;

                        Translate(velocity.x * dt, velocity.y * dt);

                        float desiredAng = desiredDir.to_angle() * RAD2DEG;
                        float currentAng = GetRotation();
                        float diff = std::fmod(desiredAng - currentAng + 540.f, 360.f) - 180.f;
                        constexpr float ROT_SPEED = 720.f;
                        float maxStep = ROT_SPEED * dt;
                        float step = std::clamp(diff, -maxStep, +maxStep);
                        SetRotation(currentAng + step);
                    }
                }
            }
        }

        if (range_to_player < ENEMY_TARGET_PLAYER_RANGE && !is_chasing)
        {
            is_chasing = true;
            if (!is_boss) SoundManager::Instance().PlaySound("hey");
        }
        bool can_attack = range_to_player < ENEMY_ATTACK_PLAYER_RANGE;

        // attack

        if (attacking)
        {
            attackRemaining -= dt;
        }
        else
        {
            if (can_attack)
            {
                attackRemaining = attackSpeed;
                SoundManager::Instance().PlaySound("swing_sword");

                auto raycast = Collision::Raycast(
                    myPos,
                    GetRotation(),
                    is_boss ? BOSS_RANGE : ENEMY_RANGE,
                    Scene::Instance().GetRawObjectPointers(),
                    Flag_Wall,
                    Flag_Enemy| Flag_Dead);

                bool player_touched = false;

                for (auto object : raycast)
                {
                    if (Entity *enemy = dynamic_cast<Entity *>(object))
                    {
                        if (!enemy->IsDead())
                        {
                            player_touched = true;
                            enemy->Damage(is_boss ? BOSS_DAMAGE : ENEMY_DAMAGE);
                        }
                    }
                }

                if (player_touched)
                {
                    SoundManager::Instance().PlaySound("hit");
                }
            }
        }

        // contrôler les animations
        if (attacking && can_chase_player)
        {
            animationSystem->SetAnimation("attack", false);
        }
        else
        {
            if (velocity.zero())
            {
                animationSystem->SetAnimation("idle", false);
            }
            else if (is_chasing)
            {
                animationSystem->SetAnimation("chase", true);
            }
            else
            {
                animationSystem->SetAnimation("walk", true);
            }
        }
    }

    void Render(SDL_Renderer *renderer,
                const Vector2D &givenPosition) override
    {
        if (debug_show_)
        {
            SDL_SetRenderDrawColor(renderer,
                                   color_.r, color_.g,
                                   color_.b, color_.a);
            SDL_Rect r;
            r.x = int(givenPosition.x - renderDelimiter.x);
            r.y = int(givenPosition.y - renderDelimiter.y);
            r.w = int(renderDelimiter.x * 2.f);
            r.h = int(renderDelimiter.y * 2.f);
            SDL_RenderFillRect(renderer, &r);
        }
        else
        {
            if (animationSystem == nullptr)
                Debug::Log("null");
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
    }

    static bool raycastHitsWall(const Vector2D &a, const Vector2D &b, const Grid &grid)
    {
        int H = grid.size();
        int W = H ? grid[0].size() : 0;
        if (!H || !W)
            return true;
        int x0 = std::clamp(int(a.x), 0, W - 1);
        int y0 = std::clamp(int(a.y), 0, H - 1);
        int x1 = std::clamp(int(b.x), 0, W - 1);
        int y1 = std::clamp(int(b.y), 0, H - 1);

        int dx = std::abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
        int dy = std::abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
        int err = dx - dy;

        while (true)
        {
            if (grid[y0][x0])
                return true;
            if (x0 == x1 && y0 == y1)
                break;
            int e2 = err * 2;
            if (e2 > -dy)
            {
                err -= dy;
                x0 += sx;
            }
            if (e2 < dx)
            {
                err += dx;
                y0 += sy;
            }
        }
        return false;
    }

private:
    void requestPathTo(const Vector2D &goalWorld)
    {
        constexpr int GRID_SIZE = 100;
        constexpr float HALF_CELLS = (GRID_SIZE - 1) / 2.0f;

        extern const float LEVEL_SIZE_FACTOR;

        auto worldToGrid = [&](const Vector2D &w)
        {
            float gx = w.x / LEVEL_SIZE_FACTOR + HALF_CELLS;
            float gy = w.y / LEVEL_SIZE_FACTOR + HALF_CELLS;
            int ix = std::clamp(int(gx), 0, GRID_SIZE - 1);
            int iy = std::clamp(int(gy), 0, GRID_SIZE - 1);
            return Vector2D{float(ix), float(iy)};
        };

        Vector2D startWorld = GetWorldPosition();
        Vector2D endWorld = goalWorld;
        Vector2D startGrid = worldToGrid(startWorld);
        Vector2D goalGrid = worldToGrid(endWorld);

        computingPath_ = true;
        pathFuture_ = std::async(
            std::launch::async,
            [this, startGrid, goalGrid, &grid = *grid_]()
            {
                auto pullString = [&](const std::vector<Vector2D> &in)
                {
                    size_t n = in.size();
                    if (n < 2)
                        return in;

                    std::vector<Vector2D> out;
                    out.reserve(n);

                    size_t i = 0;
                    out.push_back(in[0]);

                    while (i + 1 < n)
                    {
                        size_t best = i + 1;
                        for (size_t j = n - 1; j > i + 1; --j)
                        {
                            if (!raycastHitsWall(in[i], in[j], *grid_))
                            {
                                best = j;
                                break;
                            }
                        }
                        out.push_back(in[best]);
                        i = best;
                    }
                    return out;
                };

                auto gridToWorld = [&](const Vector2D &g)
                {
                    float wx = (g.x - HALF_CELLS + 0.5f) * LEVEL_SIZE_FACTOR;
                    float wy = (g.y - HALF_CELLS + 0.5f) * LEVEL_SIZE_FACTOR;
                    return Vector2D{wx, wy};
                };

                auto raw = Astar::AStar(grid, startGrid, goalGrid);
                auto clean = pullString(raw);
                std::vector<Vector2D> worldPath;
                worldPath.reserve(clean.size());
                for (auto &c : clean)
                {
                    worldPath.push_back(gridToWorld(c));
                }
                return worldPath;
            });
    }

    bool is_chasing = false, target_was_player = false;

    std::shared_ptr<Entity> player_;
    std::shared_ptr<TriggerEnemy> trigger;
    const Astar::Grid *grid_ = nullptr;

    float speed_ = ENEMY_SPEED_IDLE;
    std::vector<Vector2D> path_;
    std::vector<Vector2D> idle_points;
    size_t pathIndex_ = 0;
    std::future<std::vector<Vector2D>> pathFuture_;
    bool computingPath_ = false;

    float smoothFactor_ = 8.0f;

    SDL_Color color_{255, 0, 0, 255};
    bool debug_show_ = false;
    bool is_boss = false;
};
