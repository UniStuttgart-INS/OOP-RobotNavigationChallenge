/// @file NeutralPlayer.hpp
/// @brief Neutral Player
/// @author T. Topp (topp@ins.uni-stuttgart.de)
/// @date 2022-04-10

#pragma once

#include "internal/game/player/PlayerBase.hpp"

namespace oop::internal
{
class NeutralPlayer : public internal::PlayerBase
{
  public:
    /// @brief Default Constructor
    explicit NeutralPlayer(const ImColor& color);

    /// @brief Destructor
    ~NeutralPlayer() override = default;

    /// @brief Copy constructor
    NeutralPlayer(const NeutralPlayer&) = delete;
    /// @brief Move constructor
    NeutralPlayer(NeutralPlayer&&) = delete;
    /// @brief Copy assignment operator
    NeutralPlayer& operator=(const NeutralPlayer&) = delete;
    /// @brief Move assignment operator
    NeutralPlayer& operator=(NeutralPlayer&&) = delete;

  private:
    /// @brief Make decisions
    /// @param[in] deltaTime Time passed since last update
    void Think(float deltaTime) final;
};

} // namespace oop::internal
