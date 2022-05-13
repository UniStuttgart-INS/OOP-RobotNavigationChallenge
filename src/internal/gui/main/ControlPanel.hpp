/// @file ControlePane.hpp
/// @brief Control panel of the main window
/// @author T. Topp (topp@ins.uni-stuttgart.de)
/// @date 2022-04-01

#pragma once

namespace oop::internal
{
class ControlPanel
{
  public:
    /// @brief Default Constructor
    ControlPanel() = delete;

    /// @brief Shows the control panel
    /// @param[in] panelWidth Width in pixels
    /// @param[in] panelTotalWidth Width in pixels
    static void Draw(float panelWidth, float panelTotalWidth);
};

} // namespace oop::internal
