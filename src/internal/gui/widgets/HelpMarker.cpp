#include "HelpMarker.hpp"

#include "imgui.h"
#include <imgui_internal.h>

void oop::internal::gui::widgets::HelpMarker(const char* desc, const char* symbol) // NOLINT(clang-diagnostic-unused-function)
{
    ImGui::TextDisabled("%s", symbol);

    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0F);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

bool oop::internal::gui::widgets::BeginHelpMarker(const char* symbol)
{
    ImGui::TextDisabled("%s", symbol);

    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0F);
        return true;
    }
    return false;
}

void oop::internal::gui::widgets::EndHelpMarker()
{
    ImGui::PopTextWrapPos();
    ImGui::EndTooltip();
}