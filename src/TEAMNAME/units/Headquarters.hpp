/// @file Headquarters.hpp
/// @brief Headquarters class
/// @author T. Topp (topp@ins.uni-stuttgart.de)
/// @date 2022-04-01

#pragma once

#include "internal/game/units/HeadquartersBase.hpp"

namespace oop::TEAMNAME
{

class Headquarters : public internal::HeadquartersBase
{
  public:
    /// @brief Constructor
    Headquarters(internal::PlayerBase* parent, size_t gid, const Eigen::Vector2f& position, float heading);

    /// @brief Destructor
    ~Headquarters() override = default;

    /// @brief Copy constructor
    Headquarters(const Headquarters&) = delete;
    /// @brief Move constructor
    Headquarters(Headquarters&&) = delete;
    /// @brief Copy assignment operator
    Headquarters& operator=(const Headquarters&) = delete;
    /// @brief Move assignment operator
    Headquarters& operator=(Headquarters&&) = delete;

  private:
    /// @brief Get the type of the unit
    [[nodiscard]] std::string GetUnitTypeName() const override;

    /// @brief Make decisions
    /// @param[in] deltaTime Time passed since last update
    void Think(float deltaTime) final;
};

} // namespace oop::TEAMNAME
