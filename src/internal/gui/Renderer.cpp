#include "Renderer.hpp"

#include <imgui.h>
#include <implot.h>
#include <implot_internal.h>

#include "main/ControlPanel.hpp"
#include "main/GamePlot.hpp"
#include "menus/MainMenuBar.hpp"
#include "windows/Global.hpp"
#include "internal/game/Settings.hpp"

constexpr float CONTROL_PANEL_MIN_WIDTH = 580.0F;
constexpr float PLOT_MIN_WIDTH = 400.0F;

namespace oop::internal
{

void Renderer::OnStart()
{
    ImGui::GetStyle().FrameRounding = 4.0F;
    // ImPlot::GetStyle().Colors[ImPlotCol_ErrorBar] = ImColor(IM_COL32_BLACK);
    // ImPlot::GetStyle().Colors[ImPlotCol_ErrorBar] = ImColor(IM_COL32_WHITE);
    // ImPlot::GetStyle().Colors[ImPlotCol_AxisGrid] = ImColor(230, 230, 230, 64);
}

void Renderer::OnStop()
{
}

void Renderer::RenderFrame()
{
    MainMenuBar::Show();

    constexpr int controlPanelWidth = 350.0F;
    float plotSize = std::max(std::min(ImGui::GetContentRegionAvail().x - CONTROL_PANEL_MIN_WIDTH,
                                       ImGui::GetContentRegionAvail().y),
                              PLOT_MIN_WIDTH);

    GamePlot::Draw(plotSize);

    ImGui::SameLine();

    ImGui::BeginChild("ControlPanel");
    ControlPanel::Draw(controlPanelWidth, CONTROL_PANEL_MIN_WIDTH);
    ImGui::EndChild();

    GlobalWindows::RenderGlobalWindows();
}

} // namespace oop::internal