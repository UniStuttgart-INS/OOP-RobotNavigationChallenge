/// @file GameState.hpp
/// @brief Handles the game logic and performs an update
/// @author T. Topp (topp@ins.uni-stuttgart.de)
/// @date 2022-04-01

#pragma once

#include <array>
#include <vector>
#include <memory>
#include <variant>

#include "player/PlayerBase.hpp"
#include "resources/Resource.hpp"
#include "positioning/Satellite.hpp"

namespace oop::internal
{

class Unit;

class GameState
{
  public:
    /// @brief Default Constructor
    GameState() = delete;

    /// @brief Called when the application is started
    static void OnStart();

    /// @brief Update the game state for the time passed
    /// @param[in] deltaTime Time since last frame
    static void Update(float deltaTime);

    /// @brief Draw the current state of the game
    static void Draw();

    /// @brief Draws the Game stas
    /// @param[in] availableWidth Width in pixels
    static void DrawGameStats(float availableWidth);

  private:
    /// @brief Get the next Global Id
    static size_t GetNextGID();

    /// @brief Get a new resource position
    /// @param[in] maxDistance Maximum rectangular distance the object should have to the center position
    static Eigen::Vector2f GetNewResourcePosition(const Eigen::Vector2f& center = { 0, 0 }, float maxDistance = -1.0F);

    /// @brief Get a new satellite position and heading
    static std::pair<Eigen::Vector2f, float> GetNewSatellitePositionAndHeading();

    /// @brief List of all players (for now only one)
    static std::vector<std::shared_ptr<PlayerBase>> players;

    /// @brief List of all resources spawned on the map
    static std::vector<Resource> resources;

    /// @brief List of all satellites spawned on the map
    static std::vector<Satellite> satellites;

    /// @brief Currently selected unit by the user
    static std::variant<std::shared_ptr<const Unit>,
                        const Resource*,
                        const Satellite*>
        selectedObject;

    friend class PlayerBase;
    friend class Unit;
    friend class RobotBase;
    friend class HeadquartersBase;
    friend class Virus;
    friend class Resource;
    friend class Satellite;
};

} // namespace oop::internal