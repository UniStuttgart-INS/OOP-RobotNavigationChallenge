/// @file HelpMarker.hpp
/// @brief Text Help Marker (?) with Tooltip
/// @author T. Topp (topp@ins.uni-stuttgart.de)
/// @date 2022-04-06

#pragma once

namespace oop::internal::gui::widgets
{
/// @brief Text Help Marker, e.g. '(?)', with Tooltip
/// @param[in] desc Text to display as tooltip
/// @param[in] symbol Symbol to display
void HelpMarker(const char* desc, const char* symbol = "(?)");

/// @brief Begins a Text Help Marker, e.g. '(?)', with custom content
/// @param[in] symbol Symbol to display
bool BeginHelpMarker(const char* symbol = "(?)");

/// @brief Ends a Text Help Marker with custom content
void EndHelpMarker();

} // namespace oop::internal::gui::widgets
