/// @file PlayerBase.hpp
/// @brief Managers player objects and resources
/// @author T. Topp (topp@ins.uni-stuttgart.de)
/// @date 2022-04-05

#pragma once

#include <Eigen/Core>
#include <array>
#include <vector>
#include <memory>

#include <imgui.h>

#include "internal/game/resources/Resource.hpp"

namespace oop::internal
{
// Forward declarations
class Unit;

class PlayerBase
{
  public:
    /// @brief Constructor
    PlayerBase(const Eigen::Vector2f& position, const ImColor& color);

    /// @brief Constructor
    PlayerBase(size_t gid, Eigen::Vector2f position, const ImColor& color);

    /// @brief Destructor
    virtual ~PlayerBase() = default;

    /// @brief Copy constructor
    PlayerBase(const PlayerBase&) = delete;
    /// @brief Move constructor
    PlayerBase(PlayerBase&&) = delete;
    /// @brief Copy assignment operator
    PlayerBase& operator=(const PlayerBase&) = delete;
    /// @brief Move assignment operator
    PlayerBase& operator=(PlayerBase&&) = delete;

    // ###########################################################################################################
    //                                                  Getter
    // ###########################################################################################################

    /// @brief Get the global Id to identify the player
    [[nodiscard]] size_t GetGid() const;

    /// @brief Get the name of the player
    [[nodiscard]] const std::string& GetName() const;

    /// @brief Return the player color
    [[nodiscard]] ImColor GetColor() const;

    /// @brief Get the currently available resources of the player
    [[nodiscard]] const std::array<ResourceType, ResourceType_COUNT>& GetResources() const;

    /// @brief Get the Headquarter position
    [[nodiscard]] Eigen::Vector2f GetHeadquarterPosition() const;

    /// @brief Get the Headquarter global id
    [[nodiscard]] size_t GetHeadquarterGid() const;

    // ###########################################################################################################
    //                                                 Functions
    // ###########################################################################################################

    /// @brief Spawns the unit at the headquarters. Only works if enough resources available
    /// @param[in] unit Shared_ptr to the unit to spawn
    void SpawnUnit(const std::shared_ptr<Unit>& unit);

    // ###########################################################################################################
    //                           Protected content (only accessible from child classes)
    // ###########################################################################################################

  protected:
    /// Name of the player
    std::string m_name = "Player";

    // ###########################################################################################################
    //                                      Private content (inaccessible)
    // ###########################################################################################################
  private:
    /// @brief Make decisions
    /// @param[in] deltaTime Time passed since last update
    virtual void Think(float deltaTime) = 0;

    /// Global Id
    size_t m_gid;

    /// Position of the headquarter
    const Eigen::Vector2f m_hqPosition{ 0.0, 0.0 };

    /// Color of the player's units
    ImColor m_color{ 255, 255, 255 };

    /// List of all units the player posesses
    std::vector<std::shared_ptr<Unit>> m_units;

    /// Whether the player died
    bool m_isAlive = true;

    /// Currently available resources
    std::array<ResourceType, ResourceType_COUNT> m_resources{};

    /// Total collected resources
    std::array<ResourceType, ResourceType_COUNT> m_collectedResourcesTotal{};

    /// @brief Adds a unit to the list of game units the player posesses
    /// @param[in] unit The unit to add
    void AddUnit(const std::shared_ptr<Unit>& unit);

    friend class GameState;
    friend class Unit;
    friend class RobotBase;
    friend class HeadquartersBase;
    friend class NeutralPlayer;
};

} // namespace oop::internal
