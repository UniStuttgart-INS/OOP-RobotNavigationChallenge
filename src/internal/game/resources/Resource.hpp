/// @file Resource.hpp
/// @brief Abstract resource class
/// @author T. Topp (topp@ins.uni-stuttgart.de)
/// @date 2022-04-06

#pragma once

#include <string_view>
#include <Eigen/Core>
#include <imgui.h>
#include <fmt/format.h>

namespace oop
{

enum ResourceType : uint8_t
{
    ResourceType_Capacitor,
    ResourceType_Coil,
    ResourceType_Resistor,
    ResourceType_COUNT, // Count variable
};

namespace internal
{

class Resource
{
  public:
    /// @brief Constructor
    /// @param[in] type Type of the resource
    /// @param[in] amount Amount of resources in the object
    /// @param[in] position Position of the resource
    /// @param[in] heading Heading of the resource
    Resource(ResourceType type, int amount, Eigen::Vector2f position, float heading);

    /// @brief Get the string type of the resource
    /// @param[in] type Resource type
    static std::string GetTypeName(ResourceType type);

  private:
    /// @brief Get the string type of the resource
    [[nodiscard]] std::string GetTypeName() const;

    /// @brief Draw the resource
    void Draw() const;

    /// The size of the resource
    [[nodiscard]] float GetDrawSize() const;

    /// @brief Return the resource color
    [[nodiscard]] ImColor color() const;

    /// @brief Return the resource color for the type
    /// @param[in] type Resource type
    static ImColor color(ResourceType type);

    /// Global Id
    size_t m_gid;

    /// @brief The type of resources
    ResourceType m_type;

    /// The amount of resources available
    int m_amount = 0;

    /// @brief Position of the resource
    Eigen::Vector2f m_pos{ 0.0, 0.0 };

    /// @brief Direction of the resource measured from North in mathematical positive direction [rad]
    float m_heading = 0.0;

    friend class GameState;
    friend class Unit;
    friend class RobotBase;
};

} // namespace internal

} // namespace oop

template<>
struct fmt::formatter<oop::ResourceType> : fmt::formatter<std::string_view>
{
    auto format(oop::ResourceType type, format_context& ctx) const
        -> format_context::iterator;
};
