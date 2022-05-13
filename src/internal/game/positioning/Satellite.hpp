/// @file Satellite.hpp
/// @brief Satellite class
/// @author T. Topp (topp@ins.uni-stuttgart.de)
/// @date 2022-04-07

#pragma once

#include <Eigen/Core>
#include <imgui.h>

namespace oop::internal
{

class Satellite
{
  public:
    /// @brief Constructor
    Satellite(const Eigen::Vector2f& position, float heading);

  private:
    /// @brief Draw the satellite
    void Draw() const;

    /// @brief Updates the satellite state
    /// @param[in] deltaTime Time passed since last update
    void Update(float deltaTime);

    /// Global Id
    size_t m_gid;

    /// The size of the satellite
    static constexpr float m_size = 2.0F;

    /// The satellite color
    static const inline ImColor m_color{ 240, 230, 220 };

    /// Position of the satellite
    Eigen::Vector2f m_pos{ 0.0, 0.0 };

    /// Speed with what the satellite is moving
    float m_speed = 8.0F;

    /// @brief Direction of the satellite measured from North in mathematical positive direction [rad]
    float m_heading = 0.0;

    /// @brief When faulty it will transmit wrong info about its position
    bool m_isFaulty = false;

    /// Faulty position of the satellite
    Eigen::Vector2f m_faultyPos{ 0.0, 0.0 };

    /// Faulty Speed with what the satellite is moving
    float m_faultySpeed = 8.0F;

    friend class GameState;
    friend class Unit;
};

} // namespace oop::internal
