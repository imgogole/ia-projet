#pragma once

static constexpr int SLIDER_OUTSCALE = 2;
static constexpr int EXIT_SIZE = 50.0f;

static constexpr bool INCLUDE_ROTATION_VELOCITY = false;

static constexpr float FRAME_DURATION = 0.08f;
static constexpr float ATTACK_FRAME_DURATION = 0.03f;
static constexpr float TEXTURE_SCALE = 1.7f;
static constexpr float SQR_MIN_DISTANCE_TO_MOVE = 50.0f;
static constexpr float WALL_FRICTION = 0.3f;
static constexpr float SLOW_WHEN_ATTACK_FACTOR = 0.6f;
static constexpr int ENEMY_THRESHOLD_NEAR = 30.0f;
static constexpr int ENEMY_SPEED_IDLE = 125.0f;
static constexpr int ENEMY_SPEED_CHASE = 200.0f;
static constexpr int ENEMY_TARGET_PLAYER_RANGE = 100000.0f; // la mettre au carré !!!
static constexpr int ENEMY_ATTACK_PLAYER_RANGE = 22500.0f; // la mettre au carré !!!
static constexpr float STUCK_TIME = 0.5f;

static constexpr float PLAYER_MAX_HP = 100.0f;
static constexpr float ENEMY_MAX_HP = 75.0f;
static constexpr float BOSS_MAX_HP = 500.0f;

static constexpr float PLAYER_RANGE = 100.0f;
static constexpr float PLAYER_DAMAGE = 75.0f;
static constexpr float PLAYER_SPEED = 275.0f;
static constexpr float PLAYER_SPEED_SPRINT_MUL = 1.25f;
static constexpr float PLAYER_ATTACK_SPEED = 0.35f;

static constexpr float ENEMY_ATTACK_SPEED = 0.65f;
static constexpr float ENEMY_RANGE = 85.0f;
static constexpr float ENEMY_DAMAGE = 15.0f;

static constexpr int BOSS_SPEED_IDLE = 125.0f;
static constexpr int BOSS_SPEED_CHASE = 200.0f;
static constexpr float BOSS_ATTACK_SPEED = 0.75f;
static constexpr float BOSS_RANGE = 90.0f;
static constexpr float BOSS_DAMAGE = 35.0f;

static constexpr int BOSS_LEVEL = 4;
