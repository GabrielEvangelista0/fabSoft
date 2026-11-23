#pragma once

namespace Settings
{
constexpr int WINDOW_WIDTH = 1280;
constexpr int WINDOW_HEIGHT = 720;
constexpr int TARGET_FPS = 60;

constexpr float LANE_WIDTH = 2.2F;
constexpr float TRACK_WIDTH = 8.2F;
constexpr float TRACK_SEGMENT_LENGTH = 12.0F;
constexpr int TRACK_SEGMENT_COUNT = 16;

constexpr float PLAYER_WIDTH = 0.8F;
constexpr float PLAYER_HEIGHT = 1.8F;
constexpr float PLAYER_DEPTH = 0.8F;
constexpr float PLAYER_LANE_CHANGE_SPEED = 10.0F;
constexpr float PLAYER_JUMP_SPEED = 10.0F;
constexpr float GRAVITY = 22.0F;

constexpr float WORLD_SPEED = 11.0F;
constexpr float FOOD_RADIUS = 0.45F;
constexpr int MAX_FOODS = 64;

constexpr float MAX_WEIGHT = 100.0F;
constexpr float FOOD_SPAWN_DISTANCE = 15.0F;
constexpr float COLLISION_FLASH_DURATION = 0.35F;
constexpr float PARTICLE_LIFETIME = 0.55F;
}
