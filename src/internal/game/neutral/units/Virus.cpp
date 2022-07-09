#include "Virus.hpp"

#include <spdlog/spdlog.h>
#include <implot.h>
#include "internal/gui/helper/ImPlotHelper.hpp"
#include "internal/GameApplication.hpp"
#include "internal/game/player/PlayerBase.hpp"
#include "internal/game/GameState.hpp"
#include "internal/game/Settings.hpp"
#include "internal/helper/RandomNumberGenerator.hpp"

namespace oop::internal
{

Virus::Virus(PlayerBase* parent, const Eigen::Vector2f& position, float heading)
    : Unit(parent, GameState::GetNextGID(), position, heading)
{
    m_maxHealth = glob::units::ATTR_VIRUS_HEALTH;
    m_currentHealth = m_maxHealth;
    m_speed = glob::units::ATTR_VIRUS_SPEED;
    m_scanRange = glob::units::ATTR_VIRUS_SCAN_RANGE;
    m_attackPower = glob::units::ATTR_VIRUS_ATTACK_POWER;
    m_attackRange = glob::units::ATTR_VIRUS_ATTACK_RANGE;
}

// ###########################################################################################################
//                                                  Actions
// ###########################################################################################################

void Virus::DoMove(float heading)
{
    SetHeading(heading);
    m_action = Action_Move;
}

// ###########################################################################################################
//                                                  Getter
// ###########################################################################################################

// ###########################################################################################################
//                                      Private content (inaccessible)
// ###########################################################################################################

std::string Virus::GetUnitTypeName() const
{
    return "Virus";
}

bool Virus::IsHeadquarters() const
{
    return false;
}

void Virus::Draw() const
{
    using internal::gui::helper::PlotToPixel;

    Unit::Draw();

    auto drawBackgroundShape = [this](const ImColor& color) {
        ImPlot::GetPlotDrawList()->AddCircleFilled(ImPlot::PlotToPixels(m_pos.x(), m_pos.y()),
                                                   0.7F * static_cast<float>(PlotToPixel(GetDrawSize())), color);
    };

    if (std::holds_alternative<std::shared_ptr<const Unit>>(GameState::selectedObject)
        && std::get<std::shared_ptr<const Unit>>(GameState::selectedObject)
        && std::get<std::shared_ptr<const Unit>>(GameState::selectedObject)->m_gid == m_gid)
    {
        drawBackgroundShape(glob::gui::COLOR_SELECTED);
    }
    else if (ImPlot::IsPlotHovered()
             && (m_pos - Eigen::Vector2f{ ImPlot::GetPlotMousePos().x, ImPlot::GetPlotMousePos().y })
                        .norm()
                    <= GetDrawSize() * glob::gui::HOVER_OBJECT_SIZE_MODIFIER)
    {
        drawBackgroundShape(glob::gui::COLOR_HOVERED);
    }

    Eigen::Vector2f upperLeft = m_pos + Eigen::Vector2f{ -0.5F * GetDrawSize(), 0.5F * GetDrawSize() };
    Eigen::Vector2f lowerRright = m_pos + Eigen::Vector2f{ 0.5F * GetDrawSize(), -0.5F * GetDrawSize() };
    ImPlot::GetPlotDrawList()->AddImage(GameApplication::virusTexture,
                                        ImPlot::PlotToPixels(upperLeft.x(), upperLeft.y()),
                                        ImPlot::PlotToPixels(lowerRright.x(), lowerRright.y()));

    DrawOverlay();
}

float Virus::GetDrawSize() const
{
    return 3.0F;
}

void Virus::Update(float deltaTime)
{
    m_currentHealth += glob::units::ATTR_VIRUS_HEALTH_REGENERATION * deltaTime;
    if (m_currentHealth > m_maxHealth)
    {
        m_currentHealth = m_maxHealth;
    }

    Unit::Update(deltaTime);

    if (m_action == Action_Move)
    {
        m_pos.x() = static_cast<float>(std::clamp(m_pos.x() + m_speed * deltaTime * std::cos(M_PI_2 + m_heading),
                                                  glob::game::BOARD_WIDTH.at(0),
                                                  glob::game::BOARD_WIDTH.at(1)));

        m_pos.y() = static_cast<float>(std::clamp(m_pos.y() + m_speed * deltaTime * std::sin(M_PI_2 + m_heading),
                                                  glob::game::BOARD_HEIGHT.at(0),
                                                  glob::game::BOARD_HEIGHT.at(1)));
    }

    m_action = Action_None;
}

float Virus::CalculateHeadingToTarget() const
{
    Eigen::Vector2f diff = m_targetPos - m_pos;
    diff.x() *= -1;
    return std::atan2(diff.x(), diff.y());
}

void Virus::Think(float deltaTime)
{
    if (m_fleeTime > 0)
    {
        m_fleeTime -= deltaTime;
    }

    for (const auto& scannedUnit : ScanForUnits())
    {
        if (scannedUnit.isHQ && m_currentHealth < m_maxHealth)
        {
            spdlog::trace("Virus [{}]: Ran into headquarters. Fleeing!", GetGid());
            m_targetGid = 0;
            m_fleeTime = FLEE_TIME;
            state = State::Exploring;
            DoMove(scannedUnit.heading + static_cast<float>(M_PI));
            return;
        }
    }
    if (m_fleeTime <= 0)
    {
        for (const auto& scannedUnit : ScanForUnits())
        {
            if (scannedUnit.playerId) // Player unit
            {
                if (!m_targetGid)
                {
                    m_targetGid = scannedUnit.gid;
                    state = State::Attacking;
                    spdlog::trace("Virus [{}]: Found unit {}. Start Attacking!", GetGid(), m_targetGid);
                    break;
                }

                auto currentTarget = std::find_if(ScanForUnits().begin(), ScanForUnits().end(), [this](const UnitScanResult& res) { return res.gid == m_targetGid; });
                if (currentTarget != ScanForUnits().end())
                {
                    if (scannedUnit.distance < currentTarget->distance)
                    {
                        m_targetGid = scannedUnit.gid;
                        state = State::Attacking;
                        spdlog::trace("Virus [{}]: Switching target to {}. Start Attacking!", GetGid(), m_targetGid);
                        break;
                    }
                }
            }
        }
    }

    if (state == State::Idle)
    {
        auto heading = static_cast<float>(RandomNumberGenerator::userRngGenerator().uniform_real_distribution<>(0.0, 2.0 * M_PI));
        spdlog::trace("Virus [{}]: Exploring into direction {}°", GetGid(), heading * 180.0F / static_cast<float>(M_PI));
        m_targetGid = 0;
        state = State::Exploring;
        DoMove(heading);
        return;
    }
    if (state == State::Exploring)
    {
        constexpr double BORDER_DISTANCE = 5.0;
        // 0 Bottom, 1 Right, 2 Top, 3 Left
        int border = std::abs(m_pos.x() - glob::game::BOARD_WIDTH.at(0)) < BORDER_DISTANCE //  Left
                         ? 3
                         : (std::abs(m_pos.x() - glob::game::BOARD_WIDTH.at(1)) < BORDER_DISTANCE // Right
                                ? 1
                                : (std::abs(m_pos.y() - glob::game::BOARD_HEIGHT.at(0)) < BORDER_DISTANCE // Bottom
                                       ? 0
                                       : (std::abs(m_pos.y() - glob::game::BOARD_HEIGHT.at(1)) < BORDER_DISTANCE // Top
                                              ? 2
                                              : -1)));

        if (border >= 0)
        {
            auto heading = static_cast<float>(RandomNumberGenerator::userRngGenerator().normal_distribution<double>(-M_PI_2, M_PI_2));
            heading += static_cast<float>(border) * static_cast<float>(M_PI_2);

            spdlog::trace("Virus [{}]: Border reached [x: {}, y: {}] set new heading {}°", GetGid(), m_pos.x(), m_pos.y(), heading * 180.0F / static_cast<float>(M_PI));
            DoMove(heading);
            return;
        }

        DoMove(GetHeading());
        return;
    }
    if (state == State::Attacking)
    {
        for (const auto& scannedUnit : ScanForUnits())
        {
            if (scannedUnit.gid == m_targetGid) // Unit still in scan range
            {
                m_targetPos = m_pos
                              + scannedUnit.distance
                                    * Eigen::Vector2f{ std::cos(scannedUnit.heading + M_PI_2),
                                                       std::sin(scannedUnit.heading + M_PI_2) };

                if (scannedUnit.distance < m_attackRange && !IsReloadingWeapons())
                {
                    DoAttack(scannedUnit.gid);
                }
                else if (scannedUnit.distance > m_attackRange / 2)
                {
                    DoMove(CalculateHeadingToTarget());
                }

                return;
            }
        }
        // Unit is not in scan range anymore
        spdlog::trace("Virus [{}]: Lost unit {}. Continuing exploring.", GetGid(), m_targetGid);
        m_targetGid = 0;
        state = State::Idle;
        return;
    }
}

} // namespace oop::internal