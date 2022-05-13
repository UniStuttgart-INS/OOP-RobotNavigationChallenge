/// @file Settings.hpp
/// @brief Global settings
/// @author T. Topp (topp@ins.uni-stuttgart.de)
/// @date 2022-04-06

#pragma once

#include <array>
#include <cmath>

namespace oop::glob
{
namespace debug
{
constexpr bool SHOW_DEBUG_CONFIG = true;

bool inline DRAW_UNIT_HEALTH_BAR = false;
bool inline DRAW_UNIT_SCAN_RANGE = false;
bool inline DRAW_UNIT_COLLECT_RANGE = false;
bool inline DRAW_UNIT_ATTACK_RANGE = false;
bool inline DRAW_HQ_HEAL_RANGE = false;
bool inline DRAW_OBJECTS_IN_SCAN_RANGE = false;

bool inline DRAW_GID = false;
bool inline DRAW_SPAWN_BOUNDARIES = false;
bool inline DRAW_ENTITY_POSITIONS = false;
bool inline DRAW_SATELLITE_VISIBILITY_RANGE = false;
bool inline DRAW_SATELLITE_COUNT_ON_UNITS = false;

} // namespace debug

namespace game
{
/// @brief Number of players starting
constexpr size_t NUM_PLAYERS = 1;

/// @brief Time steps taken to update the game
float inline UPDATE_TIME_STEP = 1e-2F;

/// @brief Range for the game board width
constexpr std::array<double, 2> BOARD_WIDTH{ -100.0, 100.0 };
/// @brief Range for the game board height
constexpr std::array<double, 2> BOARD_HEIGHT{ -100.0, 100.0 };

// ------------------------------------------ Difficulty options ---------------------------------------------

/// @brief Amount of neutral units to spawn at the start of the game
constexpr int NEUTRAL_UNITS = 0;

/// @brief Enables attacking other players
constexpr bool ENABLE_PVP = false;

/// @brief Enables the random falsification of the heading when setting it (can cause the unit to jiggle)
constexpr bool ENABLE_HEADING_PRECISION = false;

/// @brief Enables a clock offset when receiving positioning distance measurements
constexpr bool ENABLE_DISTANCE_CLOCK_OFFSET = false;

/// @brief Heading will be set with that precision
constexpr float ERROR_HEADING_PRECISION = 10.0F * static_cast<float>(M_PI) / 180.0F;

/// @brief Standard deviation of the clock offset per unit
constexpr double STDDEV_POSITIONING_CLOCK_OFFSET = 2.0 /* [m] */ / 299'792'458.0;

// -----------------------------------------------------------------------------------------------------------
} // namespace game

namespace units
{
/// @brief Health of units
constexpr float ATTR_BASE_HEALTH = 100.0F;
/// @brief Speed of units
constexpr float ATTR_BASE_SPEED = 4.0F;
/// @brief Scan range of units
constexpr float ATTR_BASE_SCAN_RANGE = 10.0F;
/// @brief Resource collecting range of units
constexpr float ATTR_BASE_COLLECT_RANGE = 5.0F;
/// @brief Amount of resources the robot can collect
constexpr int ATTR_BASE_CONTAINER_SIZE = 2;
/// @brief Amount of damage the robot can deal to other units
constexpr int ATTR_BASE_ATTACK_POWER = 5;
/// @brief Range the robot can attack other units in
constexpr float ATTR_BASE_ATTACK_RANGE = 5.0F;

/// @brief Max Health of units
constexpr float ATTR_MAX_HEALTH = 100.0F;
/// @brief Max Speed of units
constexpr float ATTR_MAX_SPEED = 8.0F;
/// @brief Max Scan range of units
constexpr float ATTR_MAX_SCAN_RANGE = 20.0F;
/// @brief Max Resource collecting range of units
constexpr float ATTR_MAX_COLLECT_RANGE = 10.0F;
/// @brief Max Amount of resources the robot can collect
constexpr int ATTR_MAX_CONTAINER_SIZE = 5;
/// @brief Max Amount of damage the robot can deal to other units
constexpr int ATTR_MAX_ATTACK_POWER = 15;
/// @brief Max Range the robot can attack other units in
constexpr float ATTR_MAX_ATTACK_RANGE = 10.0F;

/// @brief Health of the HQ
constexpr float ATTR_HQ_HEALTH = 300.0F;
/// @brief Scan range of the HQ
constexpr float ATTR_HQ_SCAN_RANGE = 15.0F;
/// @brief Amount of damage the HQ can deal to other units
constexpr int ATTR_HQ_ATTACK_POWER = 10;
/// @brief Range the HQ can attack other units in
constexpr float ATTR_HQ_ATTACK_RANGE = 8.0F;
/// @brief Range the HQ can attack other units in
constexpr float ATTR_HQ_HEAL_RANGE = 3.0F;
/// @brief Amount of health units regenerate close to the HQ
constexpr float ATTR_HQ_HEAL_AMOUNT = 1.0F;

/// @brief Health of a virus
constexpr float ATTR_VIRUS_HEALTH = 200.0F;
/// @brief Speed of a virus
constexpr float ATTR_VIRUS_SPEED = 3.0F;
/// @brief Scan range of a virus
constexpr float ATTR_VIRUS_SCAN_RANGE = 11.0F;
/// @brief Amount of damage a virus can deal to other units
constexpr int ATTR_VIRUS_ATTACK_POWER = 7;
/// @brief Range a virus can attack other units in
constexpr float ATTR_VIRUS_ATTACK_RANGE = 6.0F;
/// @brief Amount of health viruses regenerate
constexpr float ATTR_VIRUS_HEALTH_REGENERATION = 0.5F;

/// @brief Time, units gets blocked after attacking from attacking again
constexpr float ATTACK_BLOCK_TIME = 1.0F;

/// @brief Speed decrease when carrying resources
constexpr float SPEED_DECREASE_WHILE_CARRYING = 2.0F;

constexpr std::array<size_t, 3> ROBOT_COSTS = {
    10, // CAPACITOR
    10, // COIL
    10, // RESISTOR
};

/// @brief Health per cost
constexpr int HEALTH_PER_COST = 5;

/// @brief Cost for health increase
constexpr std::array<size_t, 3> COSTS_PER_HEALTH = {
    1, // CAPACITOR
    0, // COIL
    0, // RESISTOR
};

/// @brief Cost for 1 speed increase
constexpr std::array<size_t, 3> COSTS_PER_SPEED = {
    0, // CAPACITOR
    0, // COIL
    3, // RESISTOR
};

/// @brief Cost for 1 scan range increase
constexpr std::array<size_t, 3> COSTS_PER_SCAN_RANGE = {
    2, // CAPACITOR
    0, // COIL
    1, // RESISTOR
};

/// @brief Cost for 1 collect range increase
constexpr std::array<size_t, 3> COSTS_PER_COLLECT_RANGE = {
    3, // CAPACITOR
    0, // COIL
    2, // RESISTOR
};

/// @brief Cost for 1 collect range increase
constexpr std::array<size_t, 3> COSTS_PER_CONTAINER_SIZE = {
    3, // CAPACITOR
    1, // COIL
    2, // RESISTOR
};

/// @brief Cost for 1 attack power increase
constexpr std::array<size_t, 3> COSTS_PER_ATTACK_POWER = {
    0, // CAPACITOR
    2, // COIL
    0, // RESISTOR
};
/// @brief Cost for 1 attack range increase
constexpr std::array<size_t, 3> COSTS_PER_ATTACK_RANGE = {
    0, // CAPACITOR
    1, // COIL
    0, // RESISTOR
};

} // namespace units

namespace positioning
{
/// @brief Amount of satellites spawning at the same time
constexpr int NUM_SAT = 6;

/// @brief Range in which satellites can be seen
constexpr float VISIBILITY_RANGE = 100.0F;

/// @brief Chance in percent to add rng bytes to the measurements
constexpr int CHANCE_FOR_RNG_BYTES = 10;

/// @brief Chance in percent to add a satellite which sends false info
constexpr int CHANCE_FOR_FALSE_CRC_SATELLITE = 10;

} // namespace positioning

namespace resources
{
constexpr float MIN_DISTANCE_RESOURCE_TO_HQ = 20.0F;
constexpr float MIN_DISTANCE_RESOURCE_TO_HQ_LIMITED = 50.0F;
constexpr int RESOURCES_ALLOWED_CLOSE_TO_HQ = 3;
constexpr float MIN_DISTANCE_RESOURCE_TO_RESOURCE = 25.0F;

constexpr std::array<size_t, 3> STARTING_RESOURCES = {
    40, // CAPACITOR
    40, // COIL
    40, // RESISTOR
};

constexpr std::array<std::array<float, 2>, 3> AMOUNT_RESOURCES_PER_ENTITY = { {
    { 20.0F, 60.0F }, // CAPACITOR
    { 20.0F, 60.0F }, // COIL
    { 20.0F, 60.0F }, // RESISTOR
} };
constexpr std::array<std::array<float, 2>, 3> AMOUNT_RESOURCES_TOTAL = { {
    { 100.0F, 300.0F }, // CAPACITOR
    { 100.0F, 300.0F }, // COIL
    { 100.0F, 300.0F }, // RESISTOR
} };

} // namespace resources

namespace gui
{
/// @brief Modifier to see if an object is hovered/clicked
constexpr float HOVER_OBJECT_SIZE_MODIFIER = 0.7F;

/// @brief Size of the hovering effect
constexpr float HOVER_BACKGROUND_WIDTH = 0.05F;

const ImColor COLOR_SELECTED{ 255, 255, 255, 160 };
const ImColor COLOR_HOVERED{ 224, 224, 224, 90 };
} // namespace gui

} // namespace oop::glob
