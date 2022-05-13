/// @file GamePlot.hpp
/// @brief The game plot of the main window
/// @author T. Topp (topp@ins.uni-stuttgart.de)
/// @date 2022-04-01

#pragma once

namespace oop::internal
{

class GamePlot
{
  public:
    /// @brief Default Constructor
    GamePlot() = delete;

    /// @brief Shows the game plot
    /// @param[in] availableWidth The size for the plot
    static void Draw(float plotSize);

  private:
    /// @brief Scale the plot axis
    static void ScalePlotAxes();
};

} // namespace oop::internal
