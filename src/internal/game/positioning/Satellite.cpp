#include "Satellite.hpp"

#include <algorithm>
#include <fmt/core.h>
#include <implot.h>
#include "internal/gui/helper/ImPlotHelper.hpp"
#include "internal/GameApplication.hpp"
#include "internal/game/GameState.hpp"
#include "internal/game/Settings.hpp"
#include "internal/helper/RandomNumberGenerator.hpp"

namespace oop::internal
{

Satellite::Satellite(const Eigen::Vector2f& position, float heading)
    : m_gid(GameState::GetNextGID()), m_pos(position), m_heading(heading), m_faultyPos(position)
{
    m_speed += RandomNumberGenerator::gameRngGenerator().normal_distribution(-0.5F, 0.5F);

    if (RandomNumberGenerator::gameRngGenerator().uniform_int_distribution<int>(0, 99) < glob::positioning::CHANCE_FOR_FALSE_CRC_SATELLITE)
    {
        m_isFaulty = true;
        m_faultySpeed += RandomNumberGenerator::gameRngGenerator().uniform_real_distribution<float>(-1.0F, 1.0F);
    }
}

void Satellite::Draw() const
{
    using oop::internal::gui::helper::PlotToPixel;

    auto drawBackgroundShape = [this](const ImColor& color) {
        ImPlot::GetPlotDrawList()->AddCircleFilled(ImPlot::PlotToPixels(m_pos.x(), m_pos.y()),
                                                   0.7F * static_cast<float>(PlotToPixel(m_size)), color);
    };

    if (std::holds_alternative<const Satellite*>(GameState::selectedObject)
        && std::get<const Satellite*>(GameState::selectedObject)
        && std::get<const Satellite*>(GameState::selectedObject)->m_gid == m_gid)
    {
        drawBackgroundShape(glob::gui::COLOR_SELECTED);
    }
    else if (ImPlot::IsPlotHovered()
             && (m_pos - Eigen::Vector2f{ ImPlot::GetPlotMousePos().x, ImPlot::GetPlotMousePos().y })
                        .norm()
                    <= m_size * glob::gui::HOVER_OBJECT_SIZE_MODIFIER)
    {
        drawBackgroundShape(glob::gui::COLOR_HOVERED);
    }

    if (m_isFaulty)
    {
        ImPlot::GetPlotDrawList()->AddCircleFilled(ImPlot::PlotToPixels(m_pos.x(), m_pos.y()),
                                                   0.8F * static_cast<float>(PlotToPixel(m_size)), ImColor{ 255, 0, 0, 50 });

        Eigen::Vector2f upperLeft = m_faultyPos + Eigen::Vector2f{ -0.2F * m_size, 0.2F * m_size };
        Eigen::Vector2f lowerRright = m_faultyPos + Eigen::Vector2f{ 0.2F * m_size, -0.2F * m_size };
        ImPlot::GetPlotDrawList()->AddImage(GameApplication::satelliteTexture,
                                            ImPlot::PlotToPixels(upperLeft.x(), upperLeft.y()),
                                            ImPlot::PlotToPixels(lowerRright.x(), lowerRright.y()));
    }

    Eigen::Vector2f upperLeft = m_pos + Eigen::Vector2f{ -0.5F * m_size, 0.5F * m_size };
    Eigen::Vector2f lowerRright = m_pos + Eigen::Vector2f{ 0.5F * m_size, -0.5F * m_size };
    ImPlot::GetPlotDrawList()->AddImage(GameApplication::satelliteTexture,
                                        ImPlot::PlotToPixels(upperLeft.x(), upperLeft.y()),
                                        ImPlot::PlotToPixels(lowerRright.x(), lowerRright.y()));

    if (glob::debug::DRAW_SATELLITE_VISIBILITY_RANGE)
    {
        ImPlot::GetPlotDrawList()->AddCircle(ImPlot::PlotToPixels(m_pos.x(), m_pos.y()), static_cast<float>(PlotToPixel(glob::positioning::VISIBILITY_RANGE)), ImColor{ 255, 229, 204 });
    }
    if (glob::debug::DRAW_ENTITY_POSITIONS)
    {
        ImPlot::GetPlotDrawList()->AddCircleFilled(ImPlot::PlotToPixels(m_pos.x(), m_pos.y()), static_cast<float>(PlotToPixel(0.1)), ImColor{ 255, 0, 0 });
    }
    if (glob::debug::DRAW_GID)
    {
        auto col = m_color.Value;
        col.w = 0.7F;
        ImPlot::PushStyleColor(ImPlotCol_InlayText, col);
        ImPlot::PlotText(fmt::format("{}", m_gid).c_str(), m_pos.x() - m_size * 3.0 / 5.0, m_pos.y() - m_size * 3.0 / 5.0);
        ImPlot::PopStyleColor();
    }
}

void Satellite::Update(float deltaTime)
{
    m_pos.x() += m_speed * deltaTime * std::cos(static_cast<float>(M_PI_2) + m_heading);
    m_pos.y() += m_speed * deltaTime * std::sin(static_cast<float>(M_PI_2) + m_heading);

    m_faultyPos.x() += m_faultySpeed * deltaTime * std::cos(static_cast<float>(M_PI_2) + m_heading);
    m_faultyPos.y() += m_faultySpeed * deltaTime * std::sin(static_cast<float>(M_PI_2) + m_heading);
}

} // namespace oop::internal