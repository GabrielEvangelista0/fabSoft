cmake_minimum_required(VERSION 3.16)

project(FujaDasCalorias VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)

include(FetchContent)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.5
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(raylib)

# Executável Principal do Jogo
add_executable(FujaDasCalorias
    src/main.cpp
    src/core/AudioManager.cpp
    src/core/GameManager.cpp
    src/entities/Food.cpp
    src/entities/Player.cpp
    src/systems/DifficultyManager.cpp
    src/systems/SaveSystem.cpp
    src/systems/ScoreSystem.cpp
    src/ui/HUD.cpp
    src/world/TrackSpawner.cpp
    src/world/FoodSpawner.cpp
)

target_include_directories(FujaDasCalorias PRIVATE src)
target_link_libraries(FujaDasCalorias PRIVATE raylib)

if(MSVC)
    target_compile_options(FujaDasCalorias PRIVATE /W4 /permissive-)
else()
    target_compile_options(FujaDasCalorias PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Suíte de Testes Unitários Automatizados
enable_testing()

add_executable(FujaDasCaloriasTests
    tests/test_game.cpp
    src/entities/Food.cpp
    src/entities/Player.cpp
    src/systems/DifficultyManager.cpp
    src/systems/SaveSystem.cpp
    src/systems/ScoreSystem.cpp
)

target_include_directories(FujaDasCaloriasTests PRIVATE src)
target_link_libraries(FujaDasCaloriasTests PRIVATE raylib)

if(MSVC)
    target_compile_options(FujaDasCaloriasTests PRIVATE /W4 /permissive-)
else()
    target_compile_options(FujaDasCaloriasTests PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_test(NAME GameMechanicsTests COMMAND FujaDasCaloriasTests)
