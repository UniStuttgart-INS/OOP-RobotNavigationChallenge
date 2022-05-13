#include "Player.hpp"

#include <spdlog/spdlog.h>
#include "../units/Robot.hpp"
#include "helper/RandomNumber.hpp"

namespace oop::TEAMNAME
{

Player::Player(const Eigen::Vector2f& position, const ImColor& color)
    : internal::PlayerBase(position, color)
{
    // Add starting units
    for (size_t i = 0; i < 3; i++)
    {
        SpawnUnit(std::make_shared<Robot>(this, Eigen::Vector2f{}, 0.0F));
    }
}

void Player::Think(float deltaTime)
{
    // TODO: Do something smart here
}

} // namespace oop::TEAMNAME
