/// @file RobotBase.hpp
/// @brief Robot base class
/// @author T. Topp (topp@ins.uni-stuttgart.de)
/// @date 2022-04-01

#pragma once

#include "Unit.hpp"

namespace oop::internal
{

class RobotBase : public Unit
{
  public:
    /// @brief Constructor
    RobotBase(PlayerBase* parent, const Eigen::Vector2f& position, float heading);

    /// @brief Destructor
    ~RobotBase() override = default;

    /// @brief Copy constructor
    RobotBase(const RobotBase&) = delete;
    /// @brief Move constructor
    RobotBase(RobotBase&&) = delete;
    /// @brief Copy assignment operator
    RobotBase& operator=(const RobotBase&) = delete;
    /// @brief Move assignment operator
    RobotBase& operator=(RobotBase&&) = delete;

    // ###########################################################################################################
    //                                                  Actions
    // ###########################################################################################################

    /// @brief Tell the unit to move into the specified direction
    /// @param[in] heading Heading in [rad] measured from North counter-clockwise
    void DoMove(float heading);

    /// @brief Tell the unit to collrect the specified resource. Only works if the unit is in range.
    /// @param[in] resourceId Global id of the resource to collect
    void DoCollectResource(size_t resourceId);

    /// @brief Drops off resources at the headquarters. Only works if the headquarters is in range.
    void DoDropOffResourcesAtHQ();

    /// @brief Throws away the resources currently carried
    void DoDiscardResources();

    // ###########################################################################################################
    //                                                  Getter
    // ###########################################################################################################

    /// @brief Scans the robot surroundings for resources
    /// @return A list of scan results
    [[nodiscard]] const std::vector<ResourceScanResult>& ScanForResources() const;

    /// @brief Returns the resources currently carried by the unit
    [[nodiscard]] const std::pair<ResourceType, size_t>& GetCarriedResources() const;

    // ###########################################################################################################
    //                                      Private content (inaccessible)
    // ###########################################################################################################
  private:
    /// @brief Checks whether the unit is the HQ
    [[nodiscard]] bool IsHeadquarters() const final;

    /// @brief Draw the unit
    void Draw() const final;

    /// The size of a robot
    [[nodiscard]] float GetDrawSize() const final;

    /// @brief Updates the unit state
    /// @param[in] deltaTime Time passed since last update
    void Update(float deltaTime) final;
};

} // namespace oop::internal
