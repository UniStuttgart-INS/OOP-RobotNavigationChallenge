#include "Unit.hpp"

#include <fmt/format.h>
#include <implot.h>
#include "internal/gui/helper/ImPlotHelper.hpp"

#include "internal/game/GameState.hpp"
#include "internal/game/Settings.hpp"
#include "internal/helper/RandomNumberGenerator.hpp"

namespace oop::internal
{

namespace hidden
{

/// CRC16-CCITT (XModem) algorithm
uint16_t computeCrc16(const unsigned char* data, size_t length)
{
    uint16_t crc = 0;

    for (uint32_t i = 0; i < length; i++)
    {
        crc = static_cast<uint16_t>((crc >> 8) | (crc << 8));

        crc = static_cast<uint16_t>(crc ^ static_cast<uint8_t>(data[i]));
        crc ^= static_cast<uint16_t>(static_cast<uint8_t>(crc & 0xFF) >> 4);
        crc ^= static_cast<uint16_t>((crc << 8) << 4);
        crc ^= static_cast<uint16_t>(((crc & 0xFF) << 4) << 1);
    }

    return crc;
}

} // namespace hidden

Unit::Unit(PlayerBase* parent, size_t gid, Eigen::Vector2f position, float heading)
    : m_gid(gid),
      m_parent(parent),
      m_pos(std::move(position)),
      m_heading(heading),
      m_speed(glob::units::ATTR_BASE_SPEED + RandomNumberGenerator::userRngGenerator().normal_distribution<float>(-0.1F, 0.1F)),
      m_maxHealth(glob::units::ATTR_BASE_HEALTH + RandomNumberGenerator::userRngGenerator().normal_distribution<float>(-1.0F, 1.0F)),
      m_scanRange(glob::units::ATTR_BASE_SCAN_RANGE + RandomNumberGenerator::userRngGenerator().normal_distribution<float>(-0.1F, 0.1F)),
      m_collectRange(glob::units::ATTR_BASE_COLLECT_RANGE + RandomNumberGenerator::userRngGenerator().normal_distribution<float>(-0.1F, 0.1F)),
      m_resourceContainerSize(glob::units::ATTR_BASE_CONTAINER_SIZE),
      m_attackPower(glob::units::ATTR_BASE_ATTACK_POWER),
      m_attackRange(glob::units::ATTR_BASE_ATTACK_RANGE + RandomNumberGenerator::userRngGenerator().normal_distribution<float>(-0.1F, 0.1F))
{
    m_currentHealth = m_maxHealth;

    if (glob::game::ENABLE_DISTANCE_CLOCK_OFFSET)
    {
        m_clockOffset = RandomNumberGenerator::userRngGenerator().normal_distribution<>(-glob::game::STDDEV_POSITIONING_CLOCK_OFFSET, glob::game::STDDEV_POSITIONING_CLOCK_OFFSET);
    }
}

// ###########################################################################################################
//                                                  Actions
// ###########################################################################################################

void Unit::DoAttack(size_t unitId)
{
    m_actionTargetGid = unitId;
    m_action = Action_Attack;
}

// ###########################################################################################################
//                                                  Getters
// ###########################################################################################################

size_t Unit::GetGid() const
{
    return m_gid;
}

Unit::Action Unit::GetCurrentAction() const
{
    return m_action;
}

float Unit::GetHeading() const
{
    return m_heading;
}

float Unit::GetHealth() const
{
    return m_currentHealth;
}

const std::vector<UnitScanResult>& Unit::ScanForUnits() const
{
    return m_currentUnitScan;
}

const std::vector<std::vector<unsigned char>>& Unit::GetSatelliteDistanceMeasurements() const
{
    return m_currentSatelliteDistanceMeasurement;
}

std::array<size_t, 3> Unit::GetUnitCosts() const
{
    auto attMods = GetUnitAttributeModifiers();

    std::array<size_t, 3> resourceCosts{};

    for (ResourceType resType = 0; resType < ResourceType_COUNT; ++resType)
    {
        resourceCosts.at(resType) = glob::units::ROBOT_COSTS.at(resType);

        resourceCosts.at(resType) += attMods.health * glob::units::COSTS_PER_HEALTH.at(resType);
        resourceCosts.at(resType) += attMods.speed * glob::units::COSTS_PER_SPEED.at(resType);
        resourceCosts.at(resType) += attMods.scanRange * glob::units::COSTS_PER_SCAN_RANGE.at(resType);
        resourceCosts.at(resType) += attMods.collectRange * glob::units::COSTS_PER_COLLECT_RANGE.at(resType);
        resourceCosts.at(resType) += attMods.containerSize * glob::units::COSTS_PER_CONTAINER_SIZE.at(resType);
        resourceCosts.at(resType) += attMods.attackPower * glob::units::COSTS_PER_ATTACK_POWER.at(resType);
        resourceCosts.at(resType) += attMods.attackRange * glob::units::COSTS_PER_ATTACK_RANGE.at(resType);
    }

    return resourceCosts;
}

bool Unit::IsReloadingWeapons() const
{
    return m_attackBlockTime > 0;
}

// -------------------------------------------- Unit Attributes ----------------------------------------------

float Unit::GetCollectRange() const
{
    return m_collectRange;
}

float Unit::GetDropOffRange() const
{
    return m_collectRange;
}

float Unit::GetAttackRange() const
{
    return m_attackRange;
}

float Unit::GetMaxHealth() const
{
    return m_maxHealth;
}

// ###########################################################################################################
//                                                  Setter
// ###########################################################################################################

void Unit::SetHeading(float heading)
{
    if (glob::game::ENABLE_HEADING_PRECISION)
    {
        heading += RandomNumberGenerator::userRngGenerator().uniform_real_distribution<float>(-glob::game::ERROR_HEADING_PRECISION / 2.0F,
                                                                                              glob::game::ERROR_HEADING_PRECISION / 2.0F);
    }

    while (heading < 0)
    {
        heading += static_cast<float>(2 * M_PI);
    }
    while (heading > 2 * M_PI)
    {
        heading -= static_cast<float>(2 * M_PI);
    }
    m_heading = heading;
}

// ###########################################################################################################
//                                      Private content (inaccessible)
// ###########################################################################################################

UnitAttributeModifiers Unit::GetUnitAttributeModifiers() const
{
    return {};
}

void Unit::ApplyUnitAttributeModifiers()
{
    auto attMods = GetUnitAttributeModifiers();

    m_maxHealth += static_cast<float>(attMods.health) * glob::units::HEALTH_PER_COST;
    m_speed += static_cast<float>(attMods.speed);
    m_scanRange += static_cast<float>(attMods.scanRange);
    m_collectRange += static_cast<float>(attMods.collectRange);
    m_resourceContainerSize += static_cast<int>(attMods.containerSize);
    m_attackPower += static_cast<int>(attMods.attackPower);
    m_attackRange += static_cast<float>(attMods.attackRange);

    m_maxHealth = std::min(m_maxHealth, glob::units::ATTR_MAX_HEALTH);
    m_speed = std::min(m_speed, glob::units::ATTR_MAX_SPEED);
    m_scanRange = std::min(m_scanRange, glob::units::ATTR_MAX_SCAN_RANGE);
    m_collectRange = std::min(m_collectRange, glob::units::ATTR_MAX_COLLECT_RANGE);
    m_resourceContainerSize = std::min(m_resourceContainerSize, glob::units::ATTR_MAX_CONTAINER_SIZE);
    m_attackPower = std::min(m_attackPower, glob::units::ATTR_MAX_ATTACK_POWER);
    m_attackRange = std::min(m_attackRange, glob::units::ATTR_MAX_ATTACK_RANGE);

    m_currentHealth = m_maxHealth;
}

void Unit::Draw() const
{
    using internal::gui::helper::PlotToPixel;

    if (glob::debug::DRAW_UNIT_HEALTH_BAR)
    {
        ImPlot::GetPlotDrawList()->AddRectFilled(ImPlot::PlotToPixels(m_pos.x() - GetDrawSize() * 3.0 / 7.0,
                                                                      m_pos.y() + GetDrawSize() * 3.0 / 4.0 + 0.1),
                                                 ImPlot::PlotToPixels(m_pos.x() + GetDrawSize() * 3.0 / 7.0,
                                                                      m_pos.y() + GetDrawSize() * 3.0 / 4.0 - 0.1),
                                                 ImColor{ 100, 100, 100 });
        ImPlot::GetPlotDrawList()->AddRectFilled(ImPlot::PlotToPixels(m_pos.x() - GetDrawSize() * 3.0 / 7.0 + 0.03,
                                                                      m_pos.y() + GetDrawSize() * 3.0 / 4.0 + 0.07),
                                                 ImPlot::PlotToPixels(m_pos.x() + (GetDrawSize() * 3.0 / 7.0 - 0.03) * (2 * m_currentHealth / m_maxHealth - 1),
                                                                      m_pos.y() + GetDrawSize() * 3.0 / 4.0 - 0.07),
                                                 ImColor{ 1.0F - m_currentHealth / m_maxHealth, m_currentHealth / m_maxHealth, 0.0F });
    }
    if (glob::debug::DRAW_GID)
    {
        auto color = m_parent->GetColor().Value;
        color.w = 0.7F;
        ImPlot::PushStyleColor(ImPlotCol_InlayText, color);
        ImPlot::PlotText(fmt::format("{}", m_gid).c_str(), m_pos.x() - GetDrawSize() * 3.0 / 5.0, m_pos.y() - GetDrawSize() * 3.0 / 5.0);
        ImPlot::PopStyleColor();
    }

    if (glob::debug::DRAW_UNIT_SCAN_RANGE)
    {
        auto color = m_parent->GetColor();
        color.Value.w = 0.4F;
        ImPlot::GetPlotDrawList()->AddCircle(ImPlot::PlotToPixels(m_pos.x(), m_pos.y()), static_cast<float>(PlotToPixel(m_scanRange)), color);
    }
    if (glob::debug::DRAW_UNIT_ATTACK_RANGE)
    {
        ImPlot::GetPlotDrawList()->AddCircle(ImPlot::PlotToPixels(m_pos.x(), m_pos.y()), static_cast<float>(PlotToPixel(m_attackRange)), ImColor{ 255, 0, 0, 120 });
    }

    if (glob::debug::DRAW_OBJECTS_IN_SCAN_RANGE)
    {
        for (const auto& scanResult : m_currentUnitScan)
        {
            Eigen::Vector2f target = m_pos + scanResult.distance * Eigen::Vector2f{ std::cos(scanResult.heading + M_PI_2), std::sin(scanResult.heading + M_PI_2) };
            auto it = std::find_if(GameState::players.begin(),
                                   GameState::players.end(),
                                   [scanResult](const std::shared_ptr<PlayerBase>& player) { return player->m_gid == scanResult.playerId; });
            ImColor col{ 153, 76, 0 };
            if (it != GameState::players.end())
            {
                col = (*it)->GetColor();
            }
            col.Value.w = 0.3F;

            ImPlot::GetPlotDrawList()->AddLine(ImPlot::PlotToPixels(m_pos.x(), m_pos.y()),
                                               ImPlot::PlotToPixels(target.x(), target.y()), col);
        }
        for (const auto& scanResult : m_currentResourceScan)
        {
            Eigen::Vector2f target = m_pos + scanResult.distance * Eigen::Vector2f{ std::cos(scanResult.heading + M_PI_2), std::sin(scanResult.heading + M_PI_2) };
            auto col = Resource::color(scanResult.type);
            col.Value.w = 0.3F;
            ImPlot::GetPlotDrawList()->AddLine(ImPlot::PlotToPixels(m_pos.x(), m_pos.y()),
                                               ImPlot::PlotToPixels(target.x(), target.y()), col);
        }
    }

    if (glob::debug::DRAW_SATELLITE_COUNT_ON_UNITS && m_parent->m_gid)
    {
        ImPlot::PushStyleColor(ImPlotCol_InlayText, m_parent->GetColor().Value);
        ImPlot::PlotText(fmt::format("{}", m_satelliteCount).c_str(), m_pos.x() + GetDrawSize() * 3.0 / 4.0, m_pos.y() + GetDrawSize() * 3.0 / 4.0);
        ImPlot::PopStyleColor();
    }

    if (glob::debug::DRAW_ENTITY_POSITIONS)
    {
        ImPlot::GetPlotDrawList()->AddCircleFilled(ImPlot::PlotToPixels(m_pos.x(), m_pos.y()), static_cast<float>(PlotToPixel(0.1)), ImColor{ 255, 0, 0 });
    }

    if (constexpr float attackAnimationDuration = 0.5F;
        m_attackBlockTime > attackAnimationDuration)
    {
        ImPlot::GetPlotDrawList()->AddLine(ImPlot::PlotToPixels(m_pos.x(), m_pos.y()),
                                           ImPlot::PlotToPixels(m_lastAttackedUnitPosition.x(), m_lastAttackedUnitPosition.y()),
                                           ImColor{ 1.0F, 0.0F, 0.0F, (m_attackBlockTime - attackAnimationDuration) / (1.0F - attackAnimationDuration) });
    }
}

void Unit::DrawOverlay() const {}

void Unit::UpdateAlways()
{
    m_currentUnitScan.clear();
    for (const auto& player : GameState::players)
    {
        for (const auto& unit : player->m_units)
        {
            if (m_gid == unit->m_gid)
            {
                continue;
            }
            Eigen::Vector2f diff = unit->m_pos - m_pos;
            auto diffNorm = diff.norm();
            if (diffNorm <= m_scanRange)
            {
                diff.x() *= -1;
                float heading = std::atan2(diff.x(), diff.y());

                m_currentUnitScan.push_back({ unit->m_parent->m_gid, unit->m_gid, heading, diffNorm, unit->m_currentHealth, unit->IsHeadquarters() });
            }
        }
    }

    if (m_parent->m_gid) // Neutral units dont need resources (for now) and satellites
    {
        m_currentResourceScan.clear();
        for (const auto& resource : GameState::resources)
        {
            Eigen::Vector2f diff = resource.m_pos - m_pos;
            auto diffNorm = diff.norm();
            if (diffNorm <= m_scanRange)
            {
                diff.x() *= -1;
                float heading = std::atan2(diff.x(), diff.y());

                m_currentResourceScan.push_back({ resource.m_gid, heading, diffNorm, resource.m_type, resource.m_amount });
            }
        }

        m_currentSatelliteDistanceMeasurement.clear();
        m_satelliteCount = 0;
        for (const auto& satellite : GameState::satellites)
        {
            if (float satUnitDistance = (m_pos - (satellite.m_isFaulty ? satellite.m_faultyPos : satellite.m_pos)).norm();
                satUnitDistance <= glob::positioning::VISIBILITY_RANGE)
            {
                if (!satellite.m_isFaulty)
                {
                    ++m_satelliteCount;
                }

                satUnitDistance += static_cast<float>(299'792'458.0 * m_clockOffset);

                constexpr size_t msgSize = 3 * sizeof(char) + 3 * sizeof(float) + sizeof(uint16_t);

                size_t rngSizeStart = 0;
                size_t rngSizeEnd = 0;
                if (RandomNumberGenerator::userRngGenerator().uniform_int_distribution<int>(0, 99) < glob::positioning::CHANCE_FOR_RNG_BYTES)
                {
                    rngSizeStart = RandomNumberGenerator::userRngGenerator().uniform_int_distribution<size_t>(0, 10);
                    rngSizeEnd = RandomNumberGenerator::userRngGenerator().uniform_int_distribution<size_t>(0, 10);
                }

                std::vector<unsigned char> measurement(rngSizeStart + msgSize + rngSizeEnd);

                for (size_t i = 0; i < rngSizeStart; i++)
                {
                    measurement.at(i) = static_cast<unsigned char>(RandomNumberGenerator::userRngGenerator().uniform_int_distribution<int>(0, 255));
                }

                measurement.at(rngSizeStart) = 'C';
                measurement.at(rngSizeStart + 1) = '+';
                measurement.at(rngSizeStart + 2) = '+';
                *reinterpret_cast<float*>(&measurement.at(rngSizeStart + 3 + 0 * 4)) = satUnitDistance;
                *reinterpret_cast<float*>(&measurement.at(rngSizeStart + 3 + 1 * 4)) = satellite.m_pos.x();
                *reinterpret_cast<float*>(&measurement.at(rngSizeStart + 3 + 2 * 4)) = satellite.m_pos.y();
                union
                {
                    uint16_t value;
                    std::array<unsigned char, 2> data;
                } crc{};
                crc.value = hidden::computeCrc16(&measurement.at(rngSizeStart), msgSize - 2);
                if (satellite.m_isFaulty)
                {
                    crc.value += RandomNumberGenerator::userRngGenerator().uniform_int_distribution<uint16_t>(1, std::numeric_limits<uint16_t>::max());
                }
                measurement.at(rngSizeStart + 3 + 3 * 4 + 0) = crc.data.at(1);
                measurement.at(rngSizeStart + 3 + 3 * 4 + 1) = crc.data.at(0);

                for (size_t i = rngSizeStart + msgSize; i < rngSizeStart + msgSize + rngSizeEnd; i++)
                {
                    measurement.at(i) = static_cast<unsigned char>(RandomNumberGenerator::userRngGenerator().uniform_int_distribution<int>(0, 255));
                }

                m_currentSatelliteDistanceMeasurement.push_back(measurement);
            }
        }
    }
}

void Unit::Update(float deltaTime)
{
    if (IsReloadingWeapons())
    {
        m_attackBlockTime -= deltaTime;
    }

    if ((m_pos - m_parent->m_units.front()->m_pos).norm() <= glob::units::ATTR_HQ_HEAL_RANGE)
    {
        m_currentHealth += glob::units::ATTR_HQ_HEAL_AMOUNT * deltaTime;
        if (m_currentHealth > m_maxHealth)
        {
            m_currentHealth = m_maxHealth;
        }
    }

    Think(deltaTime);

    m_actionTooltip = m_action;

    if (m_action == Action_Attack && !IsReloadingWeapons())
    {
        for (auto& player : GameState::players)
        {
            if (m_parent->m_gid             // This unit is a player
                && player->m_gid            // The target unit is also a player
                && !glob::game::ENABLE_PVP) // PvP is disabled
            {
                break;
            }
            for (auto& targetUnit : player->m_units)
            {
                if (m_actionTargetGid == targetUnit->m_gid
                    && (m_pos - targetUnit->m_pos).norm() <= m_attackRange)
                {
                    Eigen::Vector2f diff = targetUnit->m_pos - m_pos;
                    diff.x() *= -1;
                    m_heading = std::atan2(diff.x(), diff.y());

                    targetUnit->m_currentHealth -= static_cast<float>(m_attackPower);
                    m_attackBlockTime = glob::units::ATTACK_BLOCK_TIME;
                    m_lastAttackedUnitPosition = targetUnit->m_pos;
                    break;
                }
            }
            if (IsReloadingWeapons())
            {
                break;
            }
        }
    }
}

} // namespace oop::internal