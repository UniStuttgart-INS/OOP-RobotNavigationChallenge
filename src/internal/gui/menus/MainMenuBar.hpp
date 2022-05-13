/// @file MainMenuBar.hpp
/// @brief The main menu bar
/// @author T. Topp (topp@ins.uni-stuttgart.de)
/// @date 2022-04-01

#pragma once

namespace oop::internal
{
class MainMenuBar
{
  public:
    /// @brief Constructor
    MainMenuBar() = delete;

    /// @brief Shows the main menu bar and moves down the cursor
    static void Show();
};

} // namespace oop::internal
