#include "imgui_ex.hpp"

#include <algorithm>

namespace ImGui
{

bool SliderULong(const char* label, uint64_t* v, uint64_t v_min, uint64_t v_max, const char* format, ImGuiSliderFlags flags)
{
    return SliderScalar(label, ImGuiDataType_U64, v, &v_min, &v_max, format, flags);
}

} // namespace ImGui