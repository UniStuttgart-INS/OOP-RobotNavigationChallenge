#include "RobotBase.hpp"

#include <fmt/core.h>
#include <implot.h>
#include "internal/gui/helper/ImPlotHelper.hpp"
#include "internal/game/player/PlayerBase.hpp"
#include "internal/game/GameState.hpp"
#include "internal/game/Settings.hpp"
#include "internal/helper/RandomNumberGenerator.hpp"

namespace oop::internal
{

RobotBase::RobotBase(PlayerBase* parent, const Eigen::Vector2f& position, float heading)
    : Unit(parent, GameState::GetNextGID(), position, heading) {}

// ###########################################################################################################
//                                                  Actions
// ###########################################################################################################

void RobotBase::DoMove(float heading)
{
    SetHeading(heading);
    m_actionTargetGid = 0;
    m_action = Action_Move;
}

void RobotBase::DoCollectResource(size_t resourceId)
{
    m_actionTargetGid = resourceId;
    m_action = Action_CollectResource;
}

void RobotBase::DoDropOffResourcesAtHQ()
{
    m_actionTargetGid = m_parent->GetHeadquarterGid();
    m_action = Action_DropOffResourcesAtHQ;
}

void RobotBase::DoDiscardResources()
{
    m_actionTargetGid = 0;
    m_action = Action_DiscardResources;
}

// ###########################################################################################################
//                                                  Getter
// ###########################################################################################################

const std::vector<ResourceScanResult>& RobotBase::ScanForResources() const
{
    return m_currentResourceScan;
}

const std::pair<ResourceType, size_t>& RobotBase::GetCarriedResources() const
{
    return m_resourcesCarried;
}

// ###########################################################################################################
//                                      Private content (inaccessible)
// ###########################################################################################################

bool RobotBase::IsHeadquarters() const
{
    return false;
}

void RobotBase::Draw() const
{
    using internal::gui::helper::Rotate;
    using internal::gui::helper::PlotToPixel;

    Unit::Draw();

    auto drawBackgroundShape = [this](const ImColor& color) {
        Eigen::Vector2f M2 = m_pos + Rotate({ 0, 2.0 / 3.0 * GetDrawSize() + glob::gui::HOVER_OBJECT_SIZE_MODIFIER }, m_heading);
        Eigen::Vector2f L2 = m_pos
                             + Rotate({ -GetDrawSize() * std::tan(M_PI / 180.0 * 20) - glob::gui::HOVER_OBJECT_SIZE_MODIFIER,
                                        -1.0 / 3.0 * GetDrawSize() - glob::gui::HOVER_OBJECT_SIZE_MODIFIER },
                                      m_heading);
        Eigen::Vector2f N2 = m_pos
                             + Rotate({ GetDrawSize() * std::tan(M_PI / 180.0 * 20) + glob::gui::HOVER_OBJECT_SIZE_MODIFIER,
                                        -1.0 / 3.0 * GetDrawSize() - 0.05 },
                                      m_heading);
        ImPlot::GetPlotDrawList()->AddTriangleFilled(ImPlot::PlotToPixels(M2.x(), M2.y()),
                                                     ImPlot::PlotToPixels(N2.x(), N2.y()),
                                                     ImPlot::PlotToPixels(L2.x(), L2.y()),
                                                     color);
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

    Eigen::Vector2f M = m_pos + Rotate({ 0, 2.0 / 3.0 * GetDrawSize() }, m_heading);
    Eigen::Vector2f L = m_pos + Rotate({ -GetDrawSize() * std::tan(M_PI / 180.0 * 20), -1.0 / 3.0 * GetDrawSize() }, m_heading);
    Eigen::Vector2f N = m_pos + Rotate({ GetDrawSize() * std::tan(M_PI / 180.0 * 20), -1.0 / 3.0 * GetDrawSize() }, m_heading);
    ImPlot::GetPlotDrawList()->AddTriangleFilled(ImPlot::PlotToPixels(M.x(), M.y()),
                                                 ImPlot::PlotToPixels(N.x(), N.y()),
                                                 ImPlot::PlotToPixels(L.x(), L.y()),
                                                 m_parent->GetColor());

    if (glob::debug::DRAW_UNIT_COLLECT_RANGE)
    {
        auto color = m_parent->GetColor();
        color.Value.w = 0.2F;
        ImPlot::GetPlotDrawList()->AddCircle(ImPlot::PlotToPixels(m_pos.x(), m_pos.y()), static_cast<float>(PlotToPixel(m_collectRange)), color);
    }

    DrawOverlay();
}

float RobotBase::GetDrawSize() const
{
    return 1.5F;
}

void RobotBase::Update(float deltaTime)
{
    Unit::Update(deltaTime);

    if (m_action == Action_Move)
    {
        float speed = m_speed - (m_resourcesCarried.second ? glob::units::SPEED_DECREASE_WHILE_CARRYING : 0.0F);

        m_pos.x() = static_cast<float>(std::clamp(m_pos.x() + speed * deltaTime * std::cos(M_PI_2 + m_heading),
                                                  glob::game::BOARD_WIDTH.at(0),
                                                  glob::game::BOARD_WIDTH.at(1)));

        m_pos.y() = static_cast<float>(std::clamp(m_pos.y() + speed * deltaTime * std::sin(M_PI_2 + m_heading),
                                                  glob::game::BOARD_HEIGHT.at(0),
                                                  glob::game::BOARD_HEIGHT.at(1)));
    }
    else if (m_action == Action_CollectResource)
    {
        for (auto& resource : GameState::resources)
        {
            if (m_actionTargetGid == resource.m_gid && (m_pos - resource.m_pos).norm() <= m_collectRange)
            {
                int resourcesCollected = std::min(m_resourceContainerSize, resource.m_amount);
                resource.m_amount -= resourcesCollected;

                m_resourcesCarried = std::make_pair(resource.m_type, resourcesCollected);

                break;
            }
        }
    }
    else if (m_action == Action_DropOffResourcesAtHQ)
    {
        if ((m_pos - m_parent->GetHeadquarterPosition()).norm() <= m_collectRange)
        {
            m_parent->m_resources.at(m_resourcesCarried.first) += m_resourcesCarried.second;
            m_parent->m_collectedResourcesTotal.at(m_resourcesCarried.first) += m_resourcesCarried.second;
            m_resourcesCarried.second = 0;
        }
    }
    else if (m_action == Action_DiscardResources)
    {
        m_resourcesCarried.second = 0;
    }

    m_action = Action_None;
}

} // namespace oop::internal