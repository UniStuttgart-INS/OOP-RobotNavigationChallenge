#include "ImPlotHelper.hpp"

#include <implot.h>
#include <implot_internal.h>
#include <Eigen/Dense>

namespace oop::internal
{

double gui::helper::PlotToPixel(double plot)
{
    double plotAspect = ImPlot::GetCurrentPlot()->Axes[ImAxis_X1].GetAspect();
    return plot / plotAspect;
}

double gui::helper::PixelToPlot(double pixel)
{
    double plotAspect = ImPlot::GetCurrentPlot()->Axes[ImAxis_X1].GetAspect();
    return pixel * plotAspect;
}

Eigen::Vector2f gui::helper::Rotate(const Eigen::Vector2f& point, float heading)
{
    return Eigen::Rotation2Df{ heading } * point;
}

} // namespace oop::internal