#include "ControlPanel.hpp"

#include <imgui.h>
#include "internal/gui/widgets/imgui_ex.hpp"
#include "internal/gui/widgets/HelpMarker.hpp"

#include "internal/GameApplication.hpp"
#include "internal/game/Settings.hpp"
#include "internal/game/GameState.hpp"
#include "internal/helper/RandomNumberGenerator.hpp"

namespace oop::internal
{

void ControlPanel::Draw(float panelWidth, float panelTotalWidth)
{
    panelTotalWidth -= 3 * ImGui::GetStyle().WindowPadding.x;
    ImGui::BeginChild("ControlPanel", ImVec2{ panelTotalWidth, ImGui::GetContentRegionAvail().y }, false);

    float cursorPosX = ImGui::GetCursorPosX();
    if (GameApplication::gameFinished)
    {
        ImGui::BeginDisabled();
    }
    if (ImGui::Button(GameApplication::gameRunning
                          ? "Pause"
                          : (GameApplication::gameTime == 0 ? "Start" : "Continue"),
                      ImVec2(80.0F, 0.0F)))
    {
        GameApplication::gameRunning = !GameApplication::gameRunning;
    }
    if (GameApplication::gameFinished)
    {
        ImGui::EndDisabled();
    }

    ImGui::SameLine();
    if (ImGui::Button("Reset", ImVec2(80.0F, 0.0F)))
    {
        GameApplication::gameRunning = false;
        GameApplication::gameFinished = false;
        GameApplication::winningPlayerColor = { 0, 0, 0, 0 };

        GameApplication::gameTime = 0;
        GameState::OnStart();
    }

    ImGui::SameLine();
    ImGui::SetNextItemWidth(panelWidth - (ImGui::GetCursorPosX() - cursorPosX));
    ImGui::SliderFloat("Game Time Modifier##Slider", &GameApplication::gameTimeModifier, 0.1F, 50.0F, "%.1f", ImGuiSliderFlags_AlwaysClamp);

    cursorPosX = ImGui::GetCursorPosX();
    ImGui::Checkbox("##useSeedInsteadOfSystemTime", &RandomNumberGenerator::gameRngGenerator().useSeedInsteadOfSystemTime);
    ImGui::SameLine();
    if (!RandomNumberGenerator::gameRngGenerator().useSeedInsteadOfSystemTime)
    {
        ImGui::BeginDisabled();
    }
    ImGui::SetNextItemWidth(panelWidth - (ImGui::GetCursorPosX() - cursorPosX));
    ImGui::SliderULong("Random Number Seed", &RandomNumberGenerator::gameRngGenerator().seed, 0, std::numeric_limits<uint64_t>::max() / 2, "%lu", ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_AlwaysClamp);
    if (!RandomNumberGenerator::gameRngGenerator().useSeedInsteadOfSystemTime)
    {
        ImGui::EndDisabled();
    }
    ImGui::SameLine();
    gui::widgets::HelpMarker("CTRL+Click to edit the value manually.");

    GameState::DrawGameStats(panelTotalWidth - 2 * ImGui::GetStyle().WindowPadding.x);

    ImGui::SetNextItemWidth(80);
    int32_t gameTimeLim = GameApplication::gameTimeLimit;
    ImGui::InputInt("Game time limit [s]", &gameTimeLim, 0, 0);
    if (GameApplication::gameTimeLimit != gameTimeLim && !ImGui::IsItemActive())
    {
        GameApplication::gameTimeLimit = gameTimeLim;
    }

    if (glob::debug::SHOW_DEBUG_CONFIG)
    {
        ImGui::SetNextItemWidth(80);
        ImGui::DragFloat("Update Time step [s]", &glob::game::UPDATE_TIME_STEP, 1e-3F, 1e-3F, 1e-0F, "%.3f", ImGuiSliderFlags_AlwaysClamp);
        ImGui::SameLine();
        gui::widgets::HelpMarker("This greatly affects game performance, however it also controls the interval the game logic is triggered and can therefore change the outcome.");

        ImGui::Checkbox("Draw unit healthbar", &glob::debug::DRAW_UNIT_HEALTH_BAR);
        ImGui::Checkbox("Draw unit scan range", &glob::debug::DRAW_UNIT_SCAN_RANGE);
        ImGui::Checkbox("Draw unit collect range", &glob::debug::DRAW_UNIT_COLLECT_RANGE);
        ImGui::Checkbox("Draw unit attack range", &glob::debug::DRAW_UNIT_ATTACK_RANGE);
        ImGui::Checkbox("Draw HQ heal range", &glob::debug::DRAW_HQ_HEAL_RANGE);
        ImGui::Checkbox("Draw objects in scan range", &glob::debug::DRAW_OBJECTS_IN_SCAN_RANGE);

        ImGui::Checkbox("Draw global Id", &glob::debug::DRAW_GID);
        ImGui::Checkbox("Draw resource spawn boundaries", &glob::debug::DRAW_SPAWN_BOUNDARIES);
        ImGui::Checkbox("Draw entity positions", &glob::debug::DRAW_ENTITY_POSITIONS);
        ImGui::Checkbox("Draw satellite visibility range", &glob::debug::DRAW_SATELLITE_VISIBILITY_RANGE);
        ImGui::Checkbox("Draw satellite count on units", &glob::debug::DRAW_SATELLITE_COUNT_ON_UNITS);
    }

    ImGui::EndChild();
}

} // namespace oop::internal