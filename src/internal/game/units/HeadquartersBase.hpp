/// @file HeadquartersBase.hpp
/// @brief The headquarters unit
/// @author T. Topp (topp@ins.uni-stuttgart.de)
/// @date 2022-04-05

#pragma once

#include "Unit.hpp"

namespace oop::internal
{

class HeadquartersBase : public Unit
{
  public:
    /// @brief Constructor
    HeadquartersBase(PlayerBase* parent, size_t gid, const Eigen::Vector2f& position, float heading);

    /// @brief Destructor
    ~HeadquartersBase() override = default;

    /// @brief Copy constructor
    HeadquartersBase(const HeadquartersBase&) = delete;
    /// @brief Move constructor
    HeadquartersBase(HeadquartersBase&&) = delete;
    /// @brief Copy assignment operator
    HeadquartersBase& operator=(const HeadquartersBase&) = delete;
    /// @brief Move assignment operator
    HeadquartersBase& operator=(HeadquartersBase&&) = delete;

    // ###########################################################################################################
    //                                      Private content (inaccessible)
    // ###########################################################################################################
  private:
    /// @brief Checks whether the unit is the HQ
    [[nodiscard]] bool IsHeadquarters() const final;

    /// @brief Draw the unit
    void Draw() const final;

    /// The size of a headquarters
    [[nodiscard]] float GetDrawSize() const final;

    /// @brief Updates the unit state
    /// @param[in] deltaTime Time passed since last update
    void Update(float deltaTime) final;
};

} // namespace oop::internal
