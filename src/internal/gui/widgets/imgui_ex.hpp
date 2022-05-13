/// @file imgui_ex.hpp
/// @brief ImGui extensions
/// @author T. Topp (topp@ins.uni-stuttgart.de)
/// @date 2021-09-14

#pragma once

#include <imgui.h>

#include <limits>
#include <cstdint>

namespace ImGui
{
/// @brief Shows a Slider GUI element for 'uint64'
/// @param[in] label Label to display beside the slider. Has to be unique (use # to hide text afterwards to append an uid)
/// @param[in, out] v Pointer to the value to modify
/// @param[in] v_min Minimum value allowed
/// @param[in] v_max Maximum value allowed
/// @param[in] format Printf format to display the value with
/// @param[in] flags Slider flags to modify the behavior
/// @return True if the value was changed
bool SliderULong(const char* label, uint64_t* v, uint64_t v_min, uint64_t v_max, const char* format = "%lu", ImGuiSliderFlags flags = 0);

} // namespace ImGui