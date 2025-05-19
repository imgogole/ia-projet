#include <iostream>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#include <utilities_application.h>
#include <utilities_math.h>
#include <utilities_debug.h>
#include <utilities_audio.h>

#include <constants.h>

#include <event_inputs.h>
#include <event_collisions.h>
#include <utilities_astar.h>

#include <utilities_rect.h>
#include <utilities_time.h>
#include <utilities_random.h>
#include <utilities_text.h>
#include <utilities_animations.h>

#include <slidevalue.h>

#include <object.h>
#include <wall.h>
#include <exit.h>

#include <scene.h>
#include <camera.h>

#include <shadowplayer.h>

#include <gamelevel.h>

#include <end_video.h>
#include <triggerenemy.h>

#include <entity.h>
#include <player.h>
#include <enemy.h>

#include <mainmenu.h>

using namespace std;

typedef SDL_Window *GameWindow;
typedef SDL_Renderer *GameRenderer;
typedef TTF_Font *GameFont;

#define GAME_NAME "Ronin's Revenge"

int main()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
    {
        Debug::Error(SDL_GetError());
        return 1;
    }
    Debug::Log("Game Initialization");
    TTF_Init();
    Debug::Log("TTF Initialization");
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
    Debug::Log("IMG Initialization");

    GameWindow window = SDL_CreateWindow(
        GAME_NAME,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        1920,
        1080,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    if (!window)
    {
        Debug::Error(SDL_GetError());
        SDL_Quit();
        return 1;
    }

    GameRenderer renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!renderer)
    {
        Debug::Error(SDL_GetError());

        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    GameFont defFont = TTF_OpenFont("Fonts/Roboto.ttf", 24);

    if (!defFont)
    {
        Debug::Error(SDL_GetError());

        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        Debug::Error("Mix_OpenAudio failed: " + std::string(Mix_GetError()));
        return -1;
    }

    Time::Init();
    Random::SetSeed(time(nullptr));

    SoundManager::Instance().RegisterSound("swing_sword", "SwingSword.mp3");
    SoundManager::Instance().RegisterSound("death", "Death.mp3");
    SoundManager::Instance().RegisterSound("hit", "Hit.mp3");
    SoundManager::Instance().RegisterSound("boss", "Boss.mp3");
    SoundManager::Instance().RegisterSound("end_kill", "LastKill.mp3");
    SoundManager::Instance().RegisterSound("boss_ost", "boss_ost.mp3");
    SoundManager::Instance().RegisterSound("dojo_ost", "dojo_ost.mp3");
    SoundManager::Instance().RegisterSound("hey", "Hey.mp3");
    SoundManager::Instance().RegisterSound("main_menu", "Menu.mp3");

    SoundManager::Instance().PlaySound("main_menu", true, 0.5f);

    Scene &scene = Scene::Instance();
    Collision::CollisionSystem collisionSystem;

    auto cam = std::make_shared<Camera>();
    scene.SetCamera(cam);

    // player

    auto player = scene.CreateObject<Player>();
    player->InitPlayer(renderer);
    player->SetLayerOrder(900);
    player->SetActive(true);

    scene.SetPlayer(player);

    auto main_menu = scene.CreateObject<MainMenu>();

    // levels

    auto main_menu_lvl = scene.CreateLevel(renderer,
                                           {0, 0},
                                           {0, 0},
                                           "",
                                           {},
                                           {},
                                           "");
    main_menu_lvl->SetPlayerAllowed(false);
    main_menu->SetParent(main_menu_lvl.get());
    main_menu->Init(renderer);
    main_menu->SetActive(true);

    auto lvl0 = scene.CreateLevel(renderer,
                                  {0, 400},
                                  {0, -400},
                                  "Level0/",
                                  {"level_0.png"},
                                  {"level_0_layer.png"},
                                  "Assets/Levels/Level0/level_0_hitbox.json");

    auto lvl1 = scene.CreateLevel(renderer,
                                  {-400, 400},
                                  {435, -400},
                                  "Level1/",
                                  {"level_1.png"},
                                  {},
                                  "Assets/Levels/Level1/level_1_hitbox.json");

    auto lvl2 = scene.CreateLevel(renderer,
                                  {0, 400},
                                  {400, -440},
                                  "Level2/",
                                  {"level2_animated_0000.png", "level2_animated_0001.png", "level2_animated_0002.png", "level2_animated_0003.png", "level2_animated_0004.png", "level2_animated_0005.png", "level2_animated_0006.png"},
                                  {},
                                  "Assets/Levels/Level2/level_2_hitbox.json");

    auto lvl3 = scene.CreateLevel(renderer,
                                  {0, 400},
                                  {0, -400},
                                  "Level3/",
                                  {"level_3.png"},
                                  {},
                                  "Assets/Levels/Level3/level_3_hitbox.json");

    // enemies lvl 1

    auto e1 = scene.CreateObject<Enemy>({80, -400});
    e1->InitEnemy(renderer, {{45, -375}, {315, 0}});
    e1->SetPlayer(player);
    e1->SetGrid(lvl1->GetGrid());

    auto e2 = scene.CreateObject<Enemy>({-375, -310});
    e2->InitEnemy(renderer, {{-115, -350}, {-150, 0}});
    e2->SetPlayer(player);
    e2->SetGrid(lvl1->GetGrid());

    auto e3 = scene.CreateObject<Enemy>({150, 175});
    e3->InitEnemy(renderer, {{425, 400}, {-190, 400}, {425, 150}});
    e3->SetPlayer(player);
    e3->SetGrid(lvl1->GetGrid());

    lvl1->SetEnemies({e1, e2, e3});

    // enemies lvl 2

    auto e4 = scene.CreateObject<Enemy>({-202, -136});
    e4->InitEnemy(renderer, {});
    e4->SetPlayer(player);
    e4->SetGrid(lvl2->GetGrid());

    auto e5 = scene.CreateObject<Enemy>({155, -163});
    e5->InitEnemy(renderer, {});
    e5->SetPlayer(player);
    e5->SetGrid(lvl2->GetGrid());

    auto e6 = scene.CreateObject<Enemy>({-212, 39});
    e6->InitEnemy(renderer, {});
    e6->SetPlayer(player);
    e6->SetGrid(lvl2->GetGrid());

    auto e7 = scene.CreateObject<Enemy>({195, 117});
    e7->InitEnemy(renderer, {});
    e7->SetPlayer(player);
    e7->SetGrid(lvl2->GetGrid());

    auto e8 = scene.CreateObject<Enemy>({-404, -228});
    e8->InitEnemy(renderer, {{-400, 375}});
    e8->SetPlayer(player);
    e8->SetGrid(lvl2->GetGrid());

    auto e9 = scene.CreateObject<Enemy>({404, -228});
    e9->InitEnemy(renderer, {{400, 375}});
    e9->SetPlayer(player);
    e9->SetGrid(lvl2->GetGrid());

    lvl2->SetEnemies({e4, e5, e6, e7, e8, e9});

    // boss lvl 3

    auto boss = scene.CreateObject<Enemy>({0, -400});
    boss->InitEnemy(renderer, {}, true);
    boss->SetPlayer(player);
    boss->SetGrid(lvl3->GetGrid());

    lvl3->SetEnemies({boss});

    scene.UpdateLayerOrder();
    scene.SetLevel(0);
    bool gameRunning = true;
    SDL_Event e;

    while (gameRunning)
    {
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
                gameRunning = false;
        }

        Input::UpdateKeys(window);
        Time::Update();

        // window size
        int windowWidth, windowHeight;
        SDL_GetWindowSize(window, &windowWidth, &windowHeight);
        cam->SetViewportSize(windowWidth, windowHeight);

        // clear
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // logique du jeu
        scene.UpdateAll(Time::DeltaTime());

        // collisions
        collisionSystem.SetObjects(scene.GetRawObjectPointers());
        collisionSystem.Update();

        // rendu

        scene.RenderAll(renderer);

        // render fps text

        /*

        Text::WriteText(renderer, defFont, to_string(Time::GetAverageFPS()) + " FPS", 0, 0, Text::Anchor::TOP_RIGHT, Text::Anchor::TOP_RIGHT);

        Text::WriteText(renderer, defFont, "Position : (" + to_string(player->GetWorldPosition().x) + ", " + to_string(player->GetWorldPosition().y) + ")", 0, 0, Text::Anchor::TOP_LEFT, Text::Anchor::TOP_LEFT);
        Text::WriteText(renderer, defFont, "Auto-lock : " + to_string(player->AutoLock()), 0, 30, Text::Anchor::TOP_LEFT, Text::Anchor::TOP_LEFT);

        */

        // dessiner

        SDL_RenderPresent(renderer);

        // détruire les objects

        scene.ClearDestroyedObject();

        // evenement quitter l'application

        if (Application::AskingForQuitting())
        {
            gameRunning = false;
        }

        SDL_Delay(5);
    }

    TTF_Quit();
    IMG_Quit();

    SoundManager::Instance().CleanUp();
    Mix_CloseAudio();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}