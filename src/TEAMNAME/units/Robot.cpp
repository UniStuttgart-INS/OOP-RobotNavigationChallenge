#include "Robot.hpp"

#include <spdlog/spdlog.h>

#include <implot.h>
#include "internal/gui/helper/ImPlotHelper.hpp"
#include "helper/RandomNumber.hpp"
#include "internal/game/Settings.hpp"

#include "../player/Player.hpp"

namespace oop::TEAMNAME
{

Robot::Robot(internal::PlayerBase* parent, const Eigen::Vector2f& position, float heading)
    : internal::RobotBase(parent, position, heading) {}

std::string Robot::GetUnitTypeName() const
{
    return "Robot";
}

void Robot::DrawOverlay() const
{
}

void Robot::Think(float /* deltaTime */)
{
    // TODO: Do something smart here
}

} // namespace oop::TEAMNAME