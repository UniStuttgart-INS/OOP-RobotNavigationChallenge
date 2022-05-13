#include "Global.hpp"

#include <imgui.h>
#include <implot.h>
#include "internal/GameApplication.hpp"

namespace oop::internal
{

void GlobalWindows::RenderGlobalWindows()
{
    if (showImGuiDemoWindow)
    {
        ImGui::ShowDemoWindow();
    }
    if (showImPlotDemoWindow)
    {
        ImPlot::ShowDemoWindow();
    }

    if (GameApplication::gameFinished == 2)
    {
        if (!ImGui::IsPopupOpen("Game Finished"))
        {
            ImGui::OpenPopup("Game Finished");
        }
        if (ImGui::BeginPopupModal("Game Finished", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[2]);
            ImGui::Text("The game is finished.");

            if (GameApplication::winningPlayerColor.Value.w > 0)
            {
                ImGui::Text("Player ");
                ImGui::SameLine();
                ImGui::ColorButton("##player won color", GameApplication::winningPlayerColor.Value, ImGuiColorEditFlags_NoTooltip, ImVec2(30, 30));
                ImGui::SameLine();
                ImGui::Text("won this round!");
            }
            else
            {
                ImGui::Text("No player won this round!\nGood luck next time.");
            }

            ImGui::Indent(ImGui::GetContentRegionAvail().x / 2.0F - 25.0F);
            if (ImGui::Button("Ok"))
            {
                GameApplication::gameFinished -= 1;
                ImGui::CloseCurrentPopup();
            }
            ImGui::Unindent();

            ImGui::PopFont();
            ImGui::EndPopup();
        }
    }
}

} // namespace oop::internal
