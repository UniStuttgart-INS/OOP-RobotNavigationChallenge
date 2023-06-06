#include "Resource.hpp"

#include <fmt/core.h>
#include <implot.h>
#include "internal/gui/helper/ImPlotHelper.hpp"
#include "internal/GameApplication.hpp"
#include "internal/game/Settings.hpp"
#include "internal/game/GameState.hpp"

namespace oop::internal
{
Resource::Resource(ResourceType type, int amount, Eigen::Vector2f position, float heading)
    : m_gid(GameState::GetNextGID()), m_type(type), m_amount(amount), m_pos(std::move(position)), m_heading(heading) {}

std::string Resource::GetTypeName() const
{
    return GetTypeName(m_type);
}

std::string Resource::GetTypeName(ResourceType type)
{
    switch (type) // NOLINT(hicpp-multiway-paths-covered) False positive
    {
    case ResourceType_Capacitor:
        return "Capacitor";
    case ResourceType_Coil:
        return "Coil";
    case ResourceType_Resistor:
        return "Resistor";
    case ResourceType_COUNT:
        return {};
    }
    return {};
}

void Resource::Draw() const
{
    using oop::internal::gui::helper::Rotate;
    using oop::internal::gui::helper::PlotToPixel;

    auto drawBackgroundShape = [this](const ImColor& color) {
        Eigen::Vector2f TL = m_pos
                             + Rotate({ -0.4F * GetDrawSize() - glob::gui::HOVER_OBJECT_SIZE_MODIFIER,
                                        0.2F * GetDrawSize() + glob::gui::HOVER_OBJECT_SIZE_MODIFIER },
                                      m_heading);
        Eigen::Vector2f TR = m_pos
                             + Rotate({ 0.4F * GetDrawSize() + glob::gui::HOVER_OBJECT_SIZE_MODIFIER,
                                        0.2F * GetDrawSize() + glob::gui::HOVER_OBJECT_SIZE_MODIFIER },
                                      m_heading);
        Eigen::Vector2f BL = m_pos
                             + Rotate({ -0.4F * GetDrawSize() - glob::gui::HOVER_OBJECT_SIZE_MODIFIER,
                                        -0.2F * GetDrawSize() - glob::gui::HOVER_OBJECT_SIZE_MODIFIER },
                                      m_heading);
        Eigen::Vector2f BR = m_pos
                             + Rotate({ 0.4F * GetDrawSize() + glob::gui::HOVER_OBJECT_SIZE_MODIFIER,
                                        -0.2F * GetDrawSize() - glob::gui::HOVER_OBJECT_SIZE_MODIFIER },
                                      m_heading);
        ImPlot::GetPlotDrawList()->AddQuadFilled(ImPlot::PlotToPixels(TL.x(), TL.y()),
                                                 ImPlot::PlotToPixels(TR.x(), TR.y()),
                                                 ImPlot::PlotToPixels(BR.x(), BR.y()),
                                                 ImPlot::PlotToPixels(BL.x(), BL.y()),
                                                 color);
    };

    if (std::holds_alternative<const Resource*>(GameState::selectedObject)
        && std::get<const Resource*>(GameState::selectedObject)
        && std::get<const Resource*>(GameState::selectedObject)->m_gid == m_gid)
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

    if (m_type == ResourceType_Capacitor)
    {
        Eigen::Vector2f L = m_pos + Rotate({ -0.5F * GetDrawSize(), 0 }, m_heading);
        Eigen::Vector2f R = m_pos + Rotate({ 0.5F * GetDrawSize(), 0 }, m_heading);

        Eigen::Vector2f LC = m_pos + Rotate({ -0.1F * GetDrawSize(), 0 }, m_heading);
        Eigen::Vector2f RC = m_pos + Rotate({ 0.1F * GetDrawSize(), 0 }, m_heading);

        Eigen::Vector2f LT = m_pos + Rotate({ -0.1F * GetDrawSize(), 0.3F * GetDrawSize() }, m_heading);
        Eigen::Vector2f RT = m_pos + Rotate({ 0.1F * GetDrawSize(), 0.3F * GetDrawSize() }, m_heading);

        Eigen::Vector2f LB = m_pos + Rotate({ -0.1F * GetDrawSize(), -0.3F * GetDrawSize() }, m_heading);
        Eigen::Vector2f RB = m_pos + Rotate({ 0.1F * GetDrawSize(), -0.3F * GetDrawSize() }, m_heading);

        ImPlot::GetPlotDrawList()->AddLine(ImPlot::PlotToPixels(L.x(), L.y()), ImPlot::PlotToPixels(LC.x(), LC.y()), color(), 1.5F);
        ImPlot::GetPlotDrawList()->AddLine(ImPlot::PlotToPixels(R.x(), R.y()), ImPlot::PlotToPixels(RC.x(), RC.y()), color(), 1.5F);
        ImPlot::GetPlotDrawList()->AddLine(ImPlot::PlotToPixels(LT.x(), LT.y()), ImPlot::PlotToPixels(LB.x(), LB.y()), color(), 1.5F);
        ImPlot::GetPlotDrawList()->AddLine(ImPlot::PlotToPixels(RT.x(), RT.y()), ImPlot::PlotToPixels(RB.x(), RB.y()), color(), 1.5F);
    }
    else if (m_type == ResourceType_Coil)
    {
        Eigen::Vector2f upperLeft = m_pos + Eigen::Vector2f{ -0.5F * GetDrawSize(), 0.5F * GetDrawSize() * 417.0 / 1909.0 };
        Eigen::Vector2f lowerRright = m_pos + Eigen::Vector2f{ 0.5F * GetDrawSize(), -0.5F * GetDrawSize() * 417.0 / 1909.0 };

        ImPlot::GetPlotDrawList()->AddImage(GameApplication::coilTexture,
                                            ImPlot::PlotToPixels(upperLeft.x(), upperLeft.y()),
                                            ImPlot::PlotToPixels(lowerRright.x(), lowerRright.y()));
    }
    else if (m_type == ResourceType_Resistor)
    {
        Eigen::Vector2f L = m_pos + Rotate({ -0.5F * GetDrawSize(), 0 }, m_heading);
        Eigen::Vector2f R = m_pos + Rotate({ 0.5F * GetDrawSize(), 0 }, m_heading);

        Eigen::Vector2f LC = m_pos + Rotate({ -0.3F * GetDrawSize(), 0 }, m_heading);
        Eigen::Vector2f RC = m_pos + Rotate({ 0.3F * GetDrawSize(), 0 }, m_heading);

        Eigen::Vector2f p_min = m_pos + Rotate({ -0.3F * GetDrawSize(), 0.15F * GetDrawSize() }, m_heading);
        Eigen::Vector2f p_max = m_pos + Rotate({ 0.3F * GetDrawSize(), -0.15F * GetDrawSize() }, m_heading);

        ImPlot::GetPlotDrawList()->AddLine(ImPlot::PlotToPixels(L.x(), L.y()), ImPlot::PlotToPixels(LC.x(), LC.y()), color(), 2.0F);
        ImPlot::GetPlotDrawList()->AddLine(ImPlot::PlotToPixels(R.x(), R.y()), ImPlot::PlotToPixels(RC.x(), RC.y()), color(), 2.0F);
        ImPlot::GetPlotDrawList()->AddRect(ImPlot::PlotToPixels(p_min.x(), p_min.y()), ImPlot::PlotToPixels(p_max.x(), p_max.y()), color(), 0.0F, ImDrawFlags_None, 2.0F);
    }

    ImPlot::PushStyleColor(ImPlotCol_InlayText, color().Value);
    ImPlot::PlotText(fmt::format("{}", m_amount).c_str(), m_pos.x() + GetDrawSize() * 3.0 / 4.0, m_pos.y() + GetDrawSize() * 3.0 / 4.0);
    ImPlot::PopStyleColor();

    // ImPlot::GetPlotDrawList()->AddCircleFilled(ImPlot::PlotToPixels(m_pos.x(), m_pos.y()), PlotToPixel(0.1), ImColor{ 255, 0, 0 });

    if (glob::debug::DRAW_SPAWN_BOUNDARIES)
    {
        ImPlot::GetPlotDrawList()->AddCircle(ImPlot::PlotToPixels(m_pos.x(), m_pos.y()), static_cast<float>(PlotToPixel(glob::resources::MIN_DISTANCE_RESOURCE_TO_RESOURCE)), ImColor{ 255, 0, 0, 120 });
    }
    if (glob::debug::DRAW_ENTITY_POSITIONS)
    {
        ImPlot::GetPlotDrawList()->AddCircleFilled(ImPlot::PlotToPixels(m_pos.x(), m_pos.y()), static_cast<float>(PlotToPixel(0.1)), ImColor{ 255, 0, 0 });
    }
    if (glob::debug::DRAW_GID)
    {
        auto col = color().Value;
        col.w = 0.7F;
        ImPlot::PushStyleColor(ImPlotCol_InlayText, col);
        ImPlot::PlotText(fmt::format("{}", m_gid).c_str(), m_pos.x() - GetDrawSize() * 3.0 / 5.0, m_pos.y() - GetDrawSize() * 3.0 / 5.0);
        ImPlot::PopStyleColor();
    }
}

float Resource::GetDrawSize() const
{
    switch (m_type)
    {
    case ResourceType_Capacitor:
        return 3.0F;
    case ResourceType_Coil:
        return 5.0F;
    case ResourceType_Resistor:
        return 3.0F;
    case ResourceType_COUNT:
        return -1.0F;
    }
    return -1.0F;
}

ImColor Resource::color() const
{
    return Resource::color(m_type);
}

ImColor Resource::color(ResourceType type)
{
    switch (type) // NOLINT(hicpp-multiway-paths-covered) False positive
    {
    case ResourceType_Capacitor:
        return { 204, 204, 255 };
    case ResourceType_Coil:
        return { 125, 255, 255 };
    case ResourceType_Resistor:
        return { 229, 255, 204 };
    case ResourceType_COUNT:
        return { 0, 0, 0, 0 };
    }
    return { 0, 0, 0, 0 };
}

} // namespace oop::internal