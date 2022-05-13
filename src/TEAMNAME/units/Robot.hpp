/// @file Robot.hpp
/// @brief Robot class
/// @author T. Topp (topp@ins.uni-stuttgart.de)
/// @date 2022-04-01

#pragma once

#include <Eigen/Core>
#include "internal/game/units/RobotBase.hpp"

namespace oop::TEAMNAME
{

class Robot : public internal::RobotBase
{
  public:
    /// @brief Constructor
    Robot(internal::PlayerBase* parent, const Eigen::Vector2f& position, float heading);

    /// @brief Destructor
    ~Robot() override = default;

    /// @brief Copy constructor
    Robot(const Robot&) = delete;
    /// @brief Move constructor
    Robot(Robot&&) = delete;
    /// @brief Copy assignment operator
    Robot& operator=(const Robot&) = delete;
    /// @brief Move assignment operator
    Robot& operator=(Robot&&) = delete;

  private:
    /// @brief Get the type of the unit
    [[nodiscard]] std::string GetUnitTypeName() const override;

    /// @brief Draw an overlay over the unit (can be used for Debugging purposes)
    void DrawOverlay() const override;

    /// @brief Make decisions
    /// @param[in] deltaTime Time passed since last update
    void Think(float deltaTime) override;
};

} // namespace oop::TEAMNAME
