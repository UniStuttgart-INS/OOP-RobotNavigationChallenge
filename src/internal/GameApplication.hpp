/// @file GameApplication.hpp
/// @brief Specific Application for the GUI
/// @author T. Topp (topp@ins.uni-stuttgart.de)
/// @date 2022-04-01

#pragma once

#include <application.h>
#include <imgui.h>

namespace oop::internal
{

/// @brief Application class providing all relevant GUI callbacks
class GameApplication : public Application
{
  public:
    /// @brief Default constructor
    GameApplication() = delete;

    /// Bring the Application Constructors into this class // NOLINTNEXTLINE
    using Application::Application;

    /// @brief Called when the application is started
    void OnStart() final;

    /// @brief Called when the application is stopped
    void OnStop() final;

    /// @brief Called on every frame
    /// @param[in] deltaTime Time since last frame
    void OnFrame(float deltaTime) final;

  private:
    /// @brief Multiplicator for the game time
    static inline float gameTimeModifier = 1.0F;

    /// @brief Time of this run
    static inline float gameTime = 0.0F;

    /// @brief Flag whether the game is running
    static inline bool gameRunning = false;

    /// @brief Flag whether the game is finished
    static inline int gameFinished = 0;

    /// @brief Flag whether the game is finished
    static inline ImColor winningPlayerColor{ 0, 0, 0, 0 };

    /// @brief Texture of a coil (1909x417)
    static inline ImTextureID coilTexture = nullptr;

    /// @brief Texture of a satellite (512x512)
    static inline ImTextureID satelliteTexture = nullptr;

    /// @brief Texture of a virus (512x512)
    static inline ImTextureID virusTexture = nullptr;

    /// @brief Texture of a coil (2036x1484)
    static inline ImTextureID logoTexture = nullptr;

    friend class RandomNumberGenerator;
    friend class GlobalWindows;
    friend class ControlPanel;
    friend class GamePlot;
    friend class GameState;
    friend class Satellite;
    friend class Resource;
    friend class Virus;
};

} // namespace oop::internal