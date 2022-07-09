#include "GameApplication.hpp"

#include "gui/Renderer.hpp"
#include "game/GameState.hpp"
#include "game/Settings.hpp"

namespace oop::internal
{

void GameApplication::OnStart()
{
    coilTexture = LoadTexture("resources/coil.png");
    satelliteTexture = LoadTexture("resources/satellite.png");
    virusTexture = LoadTexture("resources/virus.png");
    logoTexture = LoadTexture("resources/INS_logo_rectangular_white_small.png");

    GameState::OnStart();
    Renderer::OnStart();
}

void GameApplication::OnStop()
{
    Renderer::OnStop();

    auto releaseTexture = [this](ImTextureID& id) {
        if (id)
        {
            DestroyTexture(id);
            id = nullptr;
        }
    };

    releaseTexture(logoTexture);
    releaseTexture(virusTexture);
    releaseTexture(satelliteTexture);
    releaseTexture(coilTexture);
}

void GameApplication::OnFrame(float deltaTime)
{
    Renderer::RenderFrame();

    float gameTimePassedThisFrame = deltaTime * gameTimeModifier;
    float dt = glob::game::UPDATE_TIME_STEP; // std::min(gameTimePassedThisFrame, glob::game::UPDATE_TIME_STEP);

    while (gameTimePassedThisFrame > 0)
    {
        GameState::Update(dt);
        if (gameRunning)
        {
            gameTime += dt;
        }

        gameTimePassedThisFrame -= dt;
    }
}

void GameApplication::BeginSlowDownGame()
{
    slowDownCounterTime = 0.0F;
    if (slowDownGameTimeModifierBackup == 0.0F)
    {
        slowDownGameTimeModifierBackup = gameTimeModifier;
        gameTimeModifier = slowDownSpeed;
    }
}

void GameApplication::EndSlowDownGame(float deltaTime)
{
    slowDownCounterTime += deltaTime;
    if (slowDownGameTimeModifierBackup != 0.0F)
    {
        if (auto slowDownDuration = glob::camera::GAME_SLOW_DOWN_DURATION * slowDownSpeed;
            slowDownCounterTime <= slowDownDuration)
        {
            gameTimeModifier = slowDownSpeed
                               + (1.0F - (slowDownDuration - slowDownCounterTime) / slowDownDuration) * (slowDownGameTimeModifierBackup - slowDownSpeed);
        }
        else
        {
            gameTimeModifier = slowDownGameTimeModifierBackup;
            slowDownGameTimeModifierBackup = 0.0F;
        }
    }
}

} // namespace oop::internal