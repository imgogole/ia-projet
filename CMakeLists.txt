cmake_minimum_required(VERSION 3.10)
project(MonJeuSDL2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)
pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)
pkg_check_modules(SDL2_MIXER REQUIRED SDL2_mixer)
pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)
pkg_check_modules(SDL2_GFX REQUIRED SDL2_gfx)

include_directories(
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_IMAGE_INCLUDE_DIRS}
    ${SDL2_MIXER_INCLUDE_DIRS}
    ${SDL2_TTF_INCLUDE_DIRS}
    ${SDL2_GFX_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/Events
    ${CMAKE_SOURCE_DIR}/Utilities
    ${CMAKE_SOURCE_DIR}/Object
    ${CMAKE_SOURCE_DIR}/Object/GameObjects
)
link_directories(
    ${SDL2_LIBRARY_DIRS}
    ${SDL2_IMAGE_LIBRARY_DIRS}
    ${SDL2_MIXER_LIBRARY_DIRS}
    ${SDL2_TTF_LIBRARY_DIRS}
    ${SDL2_GFX_LIBRARY_DIRS}
)

set(SOURCES
    ${CMAKE_SOURCE_DIR}/main.cpp
    ${CMAKE_SOURCE_DIR}/constants.h
    ${CMAKE_SOURCE_DIR}/Events/event_inputs.h
    ${CMAKE_SOURCE_DIR}/Events/event_collisions.h
    ${CMAKE_SOURCE_DIR}/Utilities/utilities_debug.h
    ${CMAKE_SOURCE_DIR}/Utilities/utilities_time.h
    ${CMAKE_SOURCE_DIR}/Utilities/utilities_random.h
    ${CMAKE_SOURCE_DIR}/Utilities/utilities_text.h
    ${CMAKE_SOURCE_DIR}/Utilities/utilities_math.h
    ${CMAKE_SOURCE_DIR}/Utilities/utilities_rect.h
    ${CMAKE_SOURCE_DIR}/Utilities/utilities_animations.h
    ${CMAKE_SOURCE_DIR}/Utilities/utilities_astar.h
    ${CMAKE_SOURCE_DIR}/Utilities/utilities_audio.h
    ${CMAKE_SOURCE_DIR}/Utilities/utilities_application.h
    ${CMAKE_SOURCE_DIR}/Object/object.h
    ${CMAKE_SOURCE_DIR}/Object/scene.h
    ${CMAKE_SOURCE_DIR}/Object/GameObjects/entity.h
    ${CMAKE_SOURCE_DIR}/Object/GameObjects/camera.h
    ${CMAKE_SOURCE_DIR}/Object/GameObjects/player.h
    ${CMAKE_SOURCE_DIR}/Object/GameObjects/wall.h
    ${CMAKE_SOURCE_DIR}/Object/GameObjects/shadowplayer.h
    ${CMAKE_SOURCE_DIR}/Object/GameObjects/gamelevel.h
    ${CMAKE_SOURCE_DIR}/Object/GameObjects/slidevalue.h
    ${CMAKE_SOURCE_DIR}/Object/GameObjects/enemy.h
    ${CMAKE_SOURCE_DIR}/Object/GameObjects/exit.h
    ${CMAKE_SOURCE_DIR}/Object/GameObjects/end_video.h
    ${CMAKE_SOURCE_DIR}/Object/GameObjects/triggerenemy.h
    ${CMAKE_SOURCE_DIR}/Object/GameObjects/mainmenu.h
)

add_executable(game ${SOURCES})

target_link_libraries(game
    ${SDL2_LIBRARIES}
    ${SDL2_IMAGE_LIBRARIES}
    ${SDL2_MIXER_LIBRARIES}
    ${SDL2_TTF_LIBRARIES}
    ${SDL2_GFX_LIBRARIES}
)