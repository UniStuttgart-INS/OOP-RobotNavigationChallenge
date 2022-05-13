#include "HeadquartersBase.hpp"

#include <memory>
#include <fmt/core.h>
#include <implot.h>
#include "internal/gui/helper/ImPlotHelper.hpp"

#include "internal/game/GameState.hpp"
#include "internal/game/Settings.hpp"
#include "internal/helper/RandomNumberGenerator.hpp"

namespace oop::internal
{

HeadquartersBase::HeadquartersBase(PlayerBase* parent, size_t gid, const Eigen::Vector2f& position, float heading)
    : Unit(parent, gid, position, heading)
{
    m_maxHealth = glob::units::ATTR_HQ_HEALTH;
    m_currentHealth = m_maxHealth;
    m_scanRange = glob::units::ATTR_HQ_SCAN_RANGE + RandomNumberGenerator::gameRngGenerator().normal_distribution(-0.1F, 0.1F);
    m_attackPower = glob::units::ATTR_HQ_ATTACK_POWER;
    m_attackRange = glob::units::ATTR_HQ_ATTACK_RANGE + RandomNumberGenerator::gameRngGenerator().normal_distribution(-0.1F, 0.1F);
}

// ###########################################################################################################
//                                      Private content (inaccessible)
// ###########################################################################################################

bool HeadquartersBase::IsHeadquarters() const
{
    return true;
}

void HeadquartersBase::Draw() const
{
    using oop::internal::gui::helper::Rotate;
    using oop::internal::gui::helper::PlotToPixel;

    Unit::Draw();

    auto drawBackgroundShape = [this](const ImColor& color) {
        Eigen::Vector2f TL = m_pos
                             + Rotate({ -0.5F * GetDrawSize() - glob::gui::HOVER_OBJECT_SIZE_MODIFIER,
                                        0.3F * GetDrawSize() + glob::gui::HOVER_OBJECT_SIZE_MODIFIER },
                                      m_heading);
        Eigen::Vector2f TR = m_pos
                             + Rotate({ 0.5F * GetDrawSize() + glob::gui::HOVER_OBJECT_SIZE_MODIFIER,
                                        0.3F * GetDrawSize() + glob::gui::HOVER_OBJECT_SIZE_MODIFIER },
                                      m_heading);
        Eigen::Vector2f BL = m_pos
                             + Rotate({ -0.5F * GetDrawSize() - glob::gui::HOVER_OBJECT_SIZE_MODIFIER,
                                        -0.3F * GetDrawSize() - glob::gui::HOVER_OBJECT_SIZE_MODIFIER },
                                      m_heading);
        Eigen::Vector2f BR = m_pos
                             + Rotate({ 0.5F * GetDrawSize() + glob::gui::HOVER_OBJECT_SIZE_MODIFIER,
                                        -0.3F * GetDrawSize() - glob::gui::HOVER_OBJECT_SIZE_MODIFIER },
                                      m_heading);
        ImPlot::GetPlotDrawList()->AddQuadFilled(ImPlot::PlotToPixels(TL.x(), TL.y()),
                                                 ImPlot::PlotToPixels(TR.x(), TR.y()),
                                                 ImPlot::PlotToPixels(BR.x(), BR.y()),
                                                 ImPlot::PlotToPixels(BL.x(), BL.y()),
                                                 color);
    };

    if (std::holds_alternative<std::shared_ptr<const Unit>>(GameState::selectedObject)
        && std::get<std::shared_ptr<const Unit>>(GameState::selectedObject)
        && std::get<std::shared_ptr<const Unit>>(GameState::selectedObject)->m_gid == m_gid)
    {
        drawBackgroundShape(glob::gui::COLOR_SELECTED);
    }
    else if (ImPlot::IsPlotHovered()
             && (m_pos
                 - Eigen::Vector2f{ ImPlot::GetPlotMousePos().x, ImPlot::GetPlotMousePos().y })
                        .norm()
                    <= GetDrawSize() * glob::gui::HOVER_OBJECT_SIZE_MODIFIER) // Click on unit
    {
        drawBackgroundShape(glob::gui::COLOR_HOVERED);
    }

    Eigen::Vector2f TL = m_pos + Rotate({ -0.5F * GetDrawSize(), 0.3F * GetDrawSize() }, m_heading);
    Eigen::Vector2f TR = m_pos + Rotate({ 0.5F * GetDrawSize(), 0.3F * GetDrawSize() }, m_heading);
    Eigen::Vector2f BL = m_pos + Rotate({ -0.5F * GetDrawSize(), -0.3F * GetDrawSize() }, m_heading);
    Eigen::Vector2f BR = m_pos + Rotate({ 0.5F * GetDrawSize(), -0.3F * GetDrawSize() }, m_heading);
    // ImPlot::GetPlotDrawList()->AddCircleFilled(ImPlot::PlotToPixels(TL.x(), TL.y()), PlotToPixel(0.1), ImColor{ 255, 0, 0 });
    // ImPlot::GetPlotDrawList()->AddCircleFilled(ImPlot::PlotToPixels(TR.x(), TR.y()), PlotToPixel(0.1), ImColor{ 0, 255, 0 });
    // ImPlot::GetPlotDrawList()->AddCircleFilled(ImPlot::PlotToPixels(BL.x(), BL.y()), PlotToPixel(0.1), ImColor{ 0, 0, 255 });
    // ImPlot::GetPlotDrawList()->AddCircleFilled(ImPlot::PlotToPixels(BR.x(), BR.y()), PlotToPixel(0.1), ImColor{ 255, 255, 255 });

    ImPlot::GetPlotDrawList()->AddQuadFilled(ImPlot::PlotToPixels(TL.x(), TL.y()),
                                             ImPlot::PlotToPixels(TR.x(), TR.y()),
                                             ImPlot::PlotToPixels(BR.x(), BR.y()),
                                             ImPlot::PlotToPixels(BL.x(), BL.y()),
                                             m_parent->GetColor());

    if (glob::debug::DRAW_SPAWN_BOUNDARIES)
    {
        ImPlot::GetPlotDrawList()->AddCircle(ImPlot::PlotToPixels(m_pos.x(), m_pos.y()), static_cast<float>(PlotToPixel(glob::resources::MIN_DISTANCE_RESOURCE_TO_HQ)), ImColor{ 255, 0, 0, 120 });
        ImPlot::GetPlotDrawList()->AddCircle(ImPlot::PlotToPixels(m_pos.x(), m_pos.y()), static_cast<float>(PlotToPixel(glob::resources::MIN_DISTANCE_RESOURCE_TO_HQ_LIMITED)), ImColor{ 255, 0, 0, 80 });
    }
    if (glob::debug::DRAW_HQ_HEAL_RANGE)
    {
        ImPlot::GetPlotDrawList()->AddCircle(ImPlot::PlotToPixels(m_pos.x(), m_pos.y()),
                                             static_cast<float>(PlotToPixel(glob::units::ATTR_HQ_HEAL_RANGE)),
                                             ImColor{ 0, 255, 0, 120 });
    }

    DrawOverlay();
}

float HeadquartersBase::GetDrawSize() const
{
    return 2.5F;
}

void HeadquartersBase::Update(float deltaTime)
{
    Unit::Update(deltaTime);

    m_action = Action_None;
}

} // namespace oop::internal