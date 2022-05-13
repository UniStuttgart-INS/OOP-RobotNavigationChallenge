/// @file Unit.hpp
/// @brief Abstract Unit class
/// @author T. Topp (topp@ins.uni-stuttgart.de)
/// @date 2022-04-01

#pragma once

#include <Eigen/Core>
#include <memory>

#include <imgui.h>
#include "internal/game/resources/Resource.hpp"

namespace oop
{

struct ResourceScanResult
{
    size_t gid{};                           ///< Global Id of the resource
    float heading{};                        ///< Heading from self to the resource found
    float distance{};                       ///< Distance from self to the resource found
    ResourceType type = ResourceType_COUNT; ///< Type of resources found
    int amount = -1;                        ///< Amount of resources
};

struct UnitScanResult
{
    size_t playerId{};  ///< Id of the player the unit belongs to
    size_t gid{};       ///< Global Id of the unit
    float heading{};    ///< Heading from self to the unit found
    float distance{};   ///< Distance from self to the unit found
    float health{};     ///< Health of the unit
    bool isHQ{ false }; ///< Flag whether the unit is a headquarters
};

struct UnitAttributeModifiers
{
    /// @brief Health of units
    size_t health = 0;
    /// @brief Speed of units
    size_t speed = 0;
    /// @brief Scan range of units
    size_t scanRange = 0;
    /// @brief Resource collecting range of units
    size_t collectRange = 0;
    /// @brief Amount of resources the robot can collect
    size_t containerSize = 0;
    /// @brief Amount of damage the robot can deal to other units
    size_t attackPower = 0;
    /// @brief Range the robot can attack other units in
    size_t attackRange = 0;
};

namespace internal
{
class PlayerBase;

class Unit
{
  public:
    /// @brief Constructor
    /// @param[in] parent Player who owns the unit
    /// @param[in] gid Global id of the unit
    /// @param[in] position Position of the unit
    /// @param[in] heading Heading of the unit
    Unit(PlayerBase* parent, size_t gid, Eigen::Vector2f position, float heading);
    /// @brief Destructor
    virtual ~Unit() = default;

    /// @brief Copy constructor
    Unit(const Unit&) = delete;
    /// @brief Move constructor
    Unit(Unit&&) = delete;
    /// @brief Copy assignment operator
    Unit& operator=(const Unit&) = delete;
    /// @brief Move assignment operator
    Unit& operator=(Unit&&) = delete;

    // ###########################################################################################################
    //                                                  Actions
    // ###########################################################################################################

    /// Possible Actions to perform every update cycle
    enum Action
    {
        Action_None,
        Action_Move,
        Action_Attack,
        Action_CollectResource,
        Action_DropOffResourcesAtHQ,
        Action_DiscardResources,
    };

    /// @brief Tell the unit to attack another unit. Only works if unit is in range.
    /// @param[in] unitId Global id of the unit to attack
    void DoAttack(size_t unitId);

    // ###########################################################################################################
    //                                                  Getters
    // ###########################################################################################################

    /// @brief Get the global Id to identify the object
    [[nodiscard]] size_t GetGid() const;

    /// @brief Get the current action
    [[nodiscard]] Action GetCurrentAction() const;

    /// @brief Returns the current heading of the unit in [rad]
    [[nodiscard]] float GetHeading() const;

    /// @brief Returns the current health of the unit [0, 100]
    [[nodiscard]] float GetHealth() const;

    /// @brief Get the Player owning the unit
    template<typename P = PlayerBase>
    P* GetPlayer()
    {
        return dynamic_cast<P*>(m_parent);
    }

    /// @brief Get the Player owning the unit
    template<typename P = PlayerBase>
    [[nodiscard]] const P* GetPlayer() const
    {
        return dynamic_cast<const P*>(m_parent);
    }

    /// @brief Scans the robot surroundings for units
    /// @return A list of scan results
    [[nodiscard]] const std::vector<UnitScanResult>& ScanForUnits() const;

    /// @brief Returns a list with Satellite Distance Measurements.
    ///
    /// A Distance measurement consists of binary data with the follwing structure.
    /// Name      ┃  Type  ┃ Value
    /// ━━━━━━━━━━╋━━━━━━━━╋━━━━━━━
    /// Sync 1    ┃ char   ┃ C
    /// Sync 2    ┃ char   ┃ +
    /// Sync 3    ┃ char   ┃ +
    /// Distance  ┃ float  ┃
    /// Sat Pos x ┃ float  ┃
    /// Sat Pos y ┃ float  ┃
    /// Checksum  ┃ uint16 ┃
    ///
    /// When turned on in the settings,
    /// - the unit receiver offset will be added to all measurements,
    /// - the satellites can append random data to the front,
    /// - the satellites can generate false messages which can be recognized by the checksum.
    [[nodiscard]] const std::vector<std::vector<unsigned char>>& GetSatelliteDistanceMeasurements() const;

    /// @brief Calculates the resource costs of this unit
    /// @return An array with [Capacitor, Coil, Resistor]
    [[nodiscard]] std::array<size_t, 3> GetUnitCosts() const;

    /// @brief Checks if the unit is reloading its weapons or it can attack
    [[nodiscard]] bool IsReloadingWeapons() const;

    // -------------------------------------------- Unit Attributes ----------------------------------------------

    /// @brief Get the resource collect range of the unit
    [[nodiscard]] float GetCollectRange() const;

    /// @brief Get the resources drop-off range of the unit
    [[nodiscard]] float GetDropOffRange() const;

    /// @brief Get the attack range of the unit
    [[nodiscard]] float GetAttackRange() const;

    /// @brief Get the maximum health of the unit
    [[nodiscard]] float GetMaxHealth() const;

    // ###########################################################################################################
    //                                                  Setter
    // ###########################################################################################################

    /// @brief Set the heading of the unit
    /// @param[in] heading Heading in [rad] measured from North counter-clockwise
    void SetHeading(float heading);

    // ###########################################################################################################
    //                                      Private content (inaccessible)
    // ###########################################################################################################
  private:
    /// @brief Get the type of the unit
    [[nodiscard]] virtual std::string GetUnitTypeName() const = 0;

    /// @brief Get the unit Attribute Modifiers. This can be overloaded to specify different unit types
    [[nodiscard]] virtual UnitAttributeModifiers GetUnitAttributeModifiers() const;

    /// @brief Apply the unit Attribute Modifiers.
    virtual void ApplyUnitAttributeModifiers();

    /// @brief Checks whether the unit is the HQ
    [[nodiscard]] virtual bool IsHeadquarters() const = 0;

    /// @brief Draw the unit
    virtual void Draw() const;

    /// @brief Draw an overlay over the unit (can be used for Debugging purposes)
    virtual void DrawOverlay() const;

    /// @brief Updates the unit state (no matter if game running or not)
    /// @param[in] deltaTime Time passed since last update
    void UpdateAlways();

    /// @brief Updates the unit state
    /// @param[in] deltaTime Time passed since last update
    virtual void Update(float deltaTime);

    /// @brief Make decisions
    /// @param[in] deltaTime Time passed since last update
    virtual void Think(float deltaTime) = 0;

    /// The standard size of units
    [[nodiscard]] virtual float GetDrawSize() const = 0;

    /// Planned action for this update
    Action m_action = Action_None;

    /// Planned action which does not get reset to show in the tooltip
    Action m_actionTooltip = Action_None;

    /// Target Global id for actions
    size_t m_actionTargetGid = 0;

    /// Global Id
    size_t m_gid;

    /// Player who owns the unit
    PlayerBase* m_parent;

    /// Position of the unit
    Eigen::Vector2f m_pos{ 0.0, 0.0 };

    /// Direction of the unit measured from North in mathematical positive direction [rad]
    float m_heading = 0.0;

    /// Health of the unit [0, m_maxHealth]
    float m_currentHealth = 100.0F;

    /// Time the unit is prevented from attacking
    float m_attackBlockTime = 0.0F;

    /// Last attacked unit (for displaying the attack)
    Eigen::Vector2f m_lastAttackedUnitPosition;

    /// Type and amount of resources currently carrying
    std::pair<ResourceType, size_t> m_resourcesCarried{};

    /// Clock offset which puts an error on the distance measurements
    double m_clockOffset = 0.0;

    /// Last unit scan
    std::vector<oop::UnitScanResult> m_currentUnitScan;

    /// Last resource scan
    std::vector<ResourceScanResult> m_currentResourceScan;

    /// Last satellite distance measurement
    std::vector<std::vector<unsigned char>> m_currentSatelliteDistanceMeasurement;

    /// Number of satellites in range to this unit (not faulty)
    size_t m_satelliteCount = 0;

    // -------------------------------------------- Unit Attributes ----------------------------------------------

    /// Speed with what the unit is moving
    float m_speed = 0.0F;
    /// Maximum Health of the unit
    float m_maxHealth = 100.0F;
    /// Range the robot can scan
    float m_scanRange = 0.0F;
    /// Range the robot can collect resources
    float m_collectRange = 0.0F;
    /// Amount of resources the robot can collect
    int m_resourceContainerSize = 0;
    /// Amount of damage the robot can deal to other units
    int m_attackPower = 0;
    /// Range the robot can attack other units in
    float m_attackRange = 0;

    // ------------------------------------------ Friend declarations --------------------------------------------

    // Makes these classes access private variables
    friend class PlayerBase;
    friend class RobotBase;
    friend class HeadquartersBase;
    friend class Virus;
    friend class GameState;
};

} // namespace internal
} // namespace oop
