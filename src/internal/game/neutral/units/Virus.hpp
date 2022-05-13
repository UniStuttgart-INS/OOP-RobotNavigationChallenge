/// @file Virus.hpp
/// @brief Virus Unit
/// @author T. Topp (topp@ins.uni-stuttgart.de)
/// @date 2022-04-10

#pragma once

#include "internal/game/units/Unit.hpp"

namespace oop::internal
{
class Virus : public Unit
{
  public:
    /// @brief Constructor
    Virus(PlayerBase* parent, const Eigen::Vector2f& position, float heading);

    /// @brief Destructor
    ~Virus() override = default;

    /// @brief Copy constructor
    Virus(const Virus&) = delete;
    /// @brief Move constructor
    Virus(Virus&&) = delete;
    /// @brief Copy assignment operator
    Virus& operator=(const Virus&) = delete;
    /// @brief Move assignment operator
    Virus& operator=(Virus&&) = delete;

    // ###########################################################################################################
    //                                                  Actions
    // ###########################################################################################################

    /// @brief Tell the unit to move into the specified direction
    /// @param[in] heading Heading in [rad] measured from North counter-clockwise
    void DoMove(float heading);

    // ###########################################################################################################
    //                                                  Getter
    // ###########################################################################################################

    // ###########################################################################################################
    //                                      Private content (inaccessible)
    // ###########################################################################################################
  private:
    /// @brief Get the type of the unit
    [[nodiscard]] std::string GetUnitTypeName() const final;

    /// @brief Checks whether the unit is the HQ
    [[nodiscard]] bool IsHeadquarters() const final;

    /// @brief Draw the unit
    void Draw() const final;

    /// The size of a robot
    [[nodiscard]] float GetDrawSize() const final;

    /// @brief Updates the unit state
    /// @param[in] deltaTime Time passed since last update
    void Update(float deltaTime) final;

    /// @brief Make decisions
    /// @param[in] deltaTime Time passed since last update
    void Think(float deltaTime) final;

    /// @brief Calculates the Heading to the target
    [[nodiscard]] float CalculateHeadingToTarget() const;

    enum class State
    {
        Idle,
        Exploring,
        Attacking,
    };

    State state = State::Idle;

    float m_fleeTime = 0.0F;

    static constexpr float FLEE_TIME = 2.0F;

    Eigen::Vector2f m_targetPos{ 0.0, 0.0 };
    size_t m_targetGid = 0;
};

} // namespace oop::internal
