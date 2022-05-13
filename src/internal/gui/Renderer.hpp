/// @file Renderer.hpp
/// @brief Main Renderer, which handles all the plotting
/// @author T. Topp (topp@ins.uni-stuttgart.de)
/// @date 2022-04-01

#pragma once

namespace oop::internal
{

class Renderer
{
  public:
    /// @brief Default Constructor
    Renderer() = delete;

    /// @brief Called when the application is started
    static void OnStart();

    /// @brief Called when the application is stopped
    static void OnStop();

    /// @brief Renders the current frame
    static void RenderFrame();
};

} // namespace oop::internal
