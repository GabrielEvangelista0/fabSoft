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

# Fontes principais agrupadas por responsabilidade.
set(FUJA_CORE_SOURCES
    src/main.cpp
    src/core/AudioManager.cpp
    src/core/EffectsSystem.cpp
    src/core/GameManager.cpp
    src/core/GameStateMachine.cpp
)

set(FUJA_ENTITY_SOURCES
    src/entities/Food.cpp
    src/entities/Player.cpp
)

set(FUJA_SYSTEM_SOURCES
    src/systems/DifficultyManager.cpp
    src/systems/SaveSystem.cpp
    src/systems/ScoreSystem.cpp
)

set(FUJA_UI_SOURCES
    src/ui/HUD.cpp
)

set(FUJA_WORLD_SOURCES
    src/world/TrackSpawner.cpp
    src/world/BuildingSpawner.cpp
    src/world/UrbanProps.cpp
    src/world/SkyEnvironment.cpp
    src/world/RoadRenderer.cpp
    src/world/RoadPropsRenderer.cpp
    src/world/FoodSystem.cpp
)

add_executable(FujaDasCalorias)
target_sources(FujaDasCalorias PRIVATE
    ${FUJA_CORE_SOURCES}
    ${FUJA_ENTITY_SOURCES}
    ${FUJA_SYSTEM_SOURCES}
    ${FUJA_UI_SOURCES}
    ${FUJA_WORLD_SOURCES}
)

target_include_directories(FujaDasCalorias PRIVATE src)
target_link_libraries(FujaDasCalorias PRIVATE raylib)

if(MSVC)
    target_compile_options(FujaDasCalorias PRIVATE /W4 /permissive-)
else()
    target_compile_options(FujaDasCalorias PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Copia automaticamente a pasta assets (fontes, etc.) para a pasta de saída do executável
add_custom_command(TARGET FujaDasCalorias POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:FujaDasCalorias>/assets"
)

# Suíte de Testes Unitários Automatizados
enable_testing()

add_executable(FujaDasCaloriasTests
    tests/test_game.cpp
    src/entities/Food.cpp
    src/entities/Player.cpp
    src/systems/DifficultyManager.cpp
    src/systems/SaveSystem.cpp
    src/systems/ScoreSystem.cpp
    src/world/FoodSystem.cpp
)

target_include_directories(FujaDasCaloriasTests PRIVATE src)
target_link_libraries(FujaDasCaloriasTests PRIVATE raylib)

if(MSVC)
    target_compile_options(FujaDasCaloriasTests PRIVATE /W4 /permissive-)
else()
    target_compile_options(FujaDasCaloriasTests PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_test(NAME GameMechanicsTests COMMAND FujaDasCaloriasTests)
