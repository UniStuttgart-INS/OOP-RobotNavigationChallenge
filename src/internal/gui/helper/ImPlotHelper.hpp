/// @file ImPlotHelper.hpp
/// @brief Functions helping with plotting in ImPlot
/// @author T. Topp (topp@ins.uni-stuttgart.de)
/// @date 2022-04-01

#pragma once

#include <Eigen/Core>

namespace oop::internal::gui::helper
{

/// @brief Converts plot coordinates into pixel coordinates
/// @param[in] plot The quantity in plot coordinates
/// @return The quantity in pixel coordinates
double PlotToPixel(double plot);

/// @brief Converts pixel coordinates into plot coordinates
/// @param[in] pixel The quantity in pixel coordinates
/// @return The quantity in plot coordinates
double PixelToPlot(double pixel);

/// @brief Rotates the given point with heading angle around the origin
/// @param[in] point Point to rotate
/// @param[in] heading Heading in [rad] measured from North in mathematical positive direction
/// @return The rotated point
Eigen::Vector2f Rotate(const Eigen::Vector2f& point, float heading);

} // namespace oop::internal::gui::helper
