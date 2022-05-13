#include "MainMenuBar.hpp"

#include <string>
#include <fmt/core.h>
#include <imgui.h>

#include "internal/gui/windows/Global.hpp"

namespace oop::internal
{

void MainMenuBar::Show()
{
    auto& io = ImGui::GetIO();

    auto cursorPosition = ImGui::GetCursorPos();
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("ImGui/ImPlot"))
        {
            ImGui::MenuItem("Show ImGui Demo Window", nullptr, &GlobalWindows::showImGuiDemoWindow);
            ImGui::MenuItem("Show ImPlot Demo Window", nullptr, &GlobalWindows::showImPlotDemoWindow);
            ImGui::EndMenu();
        }
        // Move cursor to the right, as ImGui::Spring() is not working inside menu bars
        std::string text = fmt::format("FPS: {:.2f} ({:.2g}ms)", io.Framerate, io.Framerate != 0.0F ? 1000.0F / io.Framerate : 0.0F);
        float textPosX = ImGui::GetCursorPosX() + ImGui::GetColumnWidth() - ImGui::CalcTextSize(text.c_str()).x
                         - ImGui::GetScrollX() - 2 * ImGui::GetStyle().ItemSpacing.x;
        ImGui::SetCursorPosX(textPosX);
        ImGui::Text("%s", text.c_str());

        ImGui::EndMainMenuBar();
    }
    // Move cursor down, because menu bar does not take up space
    ImGui::SetCursorPos({ cursorPosition.x, cursorPosition.y + ImGui::GetTextLineHeight() + 5 });
}

} // namespace oop::internal