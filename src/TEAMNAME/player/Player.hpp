/// @file Player.hpp
/// @brief Player object
/// @author T. Topp (topp@ins.uni-stuttgart.de)
/// @date 2022-04-08

#pragma once

#include "internal/game/player/PlayerBase.hpp"

namespace oop::TEAMNAME
{
class Player : public internal::PlayerBase
{
  public:
    /// @brief Default Constructor
    Player(const Eigen::Vector2f& position, const ImColor& color);

    /// @brief Destructor
    ~Player() override = default;

    /// @brief Copy constructor
    Player(const Player&) = delete;
    /// @brief Move constructor
    Player(Player&&) = delete;
    /// @brief Copy assignment operator
    Player& operator=(const Player&) = delete;
    /// @brief Move assignment operator
    Player& operator=(Player&&) = delete;

  private:
    /// @brief Make decisions
    /// @param[in] deltaTime Time passed since last update
    void Think(float deltaTime) override;
};

} // namespace oop::TEAMNAME
