#include "PlayerBase.hpp"

#include "spdlog/spdlog.h"
#include "internal/game/GameState.hpp"
#include "internal/game/Settings.hpp"
#include "internal/game/units/Unit.hpp"

#include "internal/helper/RandomNumberGenerator.hpp"

namespace oop::internal
{

PlayerBase::PlayerBase(const Eigen::Vector2f& position, const ImColor& color)
    : PlayerBase(GameState::GetNextGID(), position, color) {}

PlayerBase::PlayerBase(size_t gid, Eigen::Vector2f position, const ImColor& color)
    : m_gid(gid), m_hqPosition(std::move(position)), m_color(color), m_resources(glob::resources::STARTING_RESOURCES) {}

// ###########################################################################################################
//                                                  Getter
// ###########################################################################################################

size_t PlayerBase::GetGid() const
{
    return m_gid;
}

ImColor PlayerBase::GetColor() const
{
    return m_color;
}

const std::array<ResourceType, ResourceType_COUNT>& PlayerBase::GetResources() const
{
    return m_resources;
}

Eigen::Vector2f PlayerBase::GetHeadquarterPosition() const
{
    return m_hqPosition;
}

size_t PlayerBase::GetHeadquarterGid() const
{
    return m_units.front()->m_gid;
}

// ###########################################################################################################
//                                                 Functions
// ###########################################################################################################

void PlayerBase::SpawnUnit(const std::shared_ptr<Unit>& unit)
{
    auto resCosts = unit->GetUnitCosts();

    for (ResourceType resType = 0; resType < ResourceType_COUNT; ++resType)
    {
        if (resCosts.at(resType) > m_resources.at(resType))
        {
            spdlog::warn("Player {} can't spawn unit because not enough {}", m_gid, Resource::GetTypeName(resType));
            return;
        }
    }

    // Spawing possible
    for (ResourceType resType = 0; resType < ResourceType_COUNT; ++resType)
    {
        m_resources.at(resType) -= resCosts.at(resType);
    }

    unit->ApplyUnitAttributeModifiers();

    auto heading = RandomNumberGenerator::userRngGenerator().uniform_real_distribution<float>(0, 2.0F * static_cast<float>(M_PI));
    Eigen::Vector2f position = m_hqPosition + 3.0F * Eigen::Vector2f{ std::cos(M_PI_2 + heading), std::sin(M_PI_2 + heading) };

    unit->m_heading = heading;
    unit->m_pos = position;

    spdlog::info("Unit [{}] spawned with heading {}", m_gid, heading * 180 / M_PI);

    AddUnit(unit);
}

// ###########################################################################################################
//                                      Private content (inaccessible)
// ###########################################################################################################

void PlayerBase::AddUnit(const std::shared_ptr<Unit>& unit)
{
    m_units.push_back(unit);
}

} // namespace oop::internal