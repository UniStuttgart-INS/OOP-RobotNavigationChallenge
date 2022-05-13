#include "Headquarters.hpp"

#include <memory>
#include "../units/Robot.hpp"
#include "../player/Player.hpp"

namespace oop::TEAMNAME
{

Headquarters::Headquarters(internal::PlayerBase* parent, size_t gid, const Eigen::Vector2f& position, float heading)
    : internal::HeadquartersBase(parent, gid, position, heading)
{
}

std::string Headquarters::GetUnitTypeName() const
{
    return "Headquarters";
}

void Headquarters::Think(float /* deltaTime */)
{
    // TODO: Do something smart here
}

} // namespace oop::TEAMNAME