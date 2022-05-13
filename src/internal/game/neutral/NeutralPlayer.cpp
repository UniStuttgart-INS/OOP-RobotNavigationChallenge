#include "NeutralPlayer.hpp"

#include <fmt/core.h>
#include "units/Virus.hpp"
#include "internal/game/Settings.hpp"
#include "internal/helper/RandomNumberGenerator.hpp"

namespace oop::internal
{

NeutralPlayer::NeutralPlayer(const ImColor& color)
    : internal::PlayerBase(0, Eigen::Vector2f{}, color)
{
    // Add starting units
    for (size_t i = 0; i < glob::game::NEUTRAL_UNITS; i++)
    {
        Eigen::Vector2f startPosition = Eigen::Vector2f{ RandomNumberGenerator::gameRngGenerator().normal_distribution<double>(0.1, (glob::game::BOARD_WIDTH.at(1) - glob::game::BOARD_WIDTH.at(0)) * 0.1 - 0.1),
                                                         RandomNumberGenerator::gameRngGenerator().normal_distribution<double>(0.1, (glob::game::BOARD_HEIGHT.at(1) - glob::game::BOARD_HEIGHT.at(0)) * 0.1 - 0.1) };

        if (startPosition.x() > (glob::game::BOARD_WIDTH.at(1) - glob::game::BOARD_WIDTH.at(0)) * 0.05)
        {
            startPosition.x() += static_cast<float>((glob::game::BOARD_WIDTH.at(1) - glob::game::BOARD_WIDTH.at(0)) * 0.9);
        }
        if (startPosition.y() > (glob::game::BOARD_HEIGHT.at(1) - glob::game::BOARD_HEIGHT.at(0)) * 0.05)
        {
            startPosition.y() += static_cast<float>((glob::game::BOARD_HEIGHT.at(1) - glob::game::BOARD_WIDTH.at(0)) * 0.9);
        }

        startPosition += Eigen::Vector2f{ glob::game::BOARD_WIDTH.at(0), glob::game::BOARD_HEIGHT.at(0) };

        AddUnit(std::make_shared<Virus>(this, startPosition, 0.0F));
    }

    m_resources.fill(0);
}

void NeutralPlayer::Think(float /* deltaTime */)
{
}

} // namespace oop::internal
