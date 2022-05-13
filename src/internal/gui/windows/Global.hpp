/// @file Global.hpp
/// @brief Global windows
/// @author T. Topp (topp@ins.uni-stuttgart.de)
/// @date 2022-04-01

#pragma once

namespace oop::internal
{
class GlobalWindows
{
  public:
    /// @brief Default Constructor
    GlobalWindows() = delete;

    /// @brief Called every frame to render global windows
    static void RenderGlobalWindows();

  private:
    /// @brief Flag whether the ImGui Demo window should be displayed
    static inline bool showImGuiDemoWindow = false;
    /// @brief Flag whether the ImPlot Demo window should be displayed
    static inline bool showImPlotDemoWindow = false;

    friend class MainMenuBar;
};

} // namespace oop::internal