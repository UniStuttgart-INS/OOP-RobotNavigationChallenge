#include "GamePlot.hpp"

#include <algorithm>

#include <implot.h>
#include <implot_internal.h>

#include "internal/game/GameState.hpp"
#include "internal/game/Settings.hpp"
#include "internal/GameApplication.hpp"

namespace oop::internal
{

void GamePlot::ScalePlotAxes()
{
    double x_min = std::clamp(ImPlot::GetCurrentPlot()->Axes[ImAxis_X1].Range.Min, glob::game::BOARD_WIDTH.at(0), glob::game::BOARD_WIDTH.at(1));
    double x_max = std::clamp(ImPlot::GetCurrentPlot()->Axes[ImAxis_X1].Range.Max, glob::game::BOARD_WIDTH.at(0), glob::game::BOARD_WIDTH.at(1));
    double y_min = std::clamp(ImPlot::GetCurrentPlot()->Axes[ImAxis_Y1].Range.Min, glob::game::BOARD_HEIGHT.at(0), glob::game::BOARD_HEIGHT.at(1));
    double y_max = std::clamp(ImPlot::GetCurrentPlot()->Axes[ImAxis_Y1].Range.Max, glob::game::BOARD_HEIGHT.at(0), glob::game::BOARD_HEIGHT.at(1));

    if (ImPlot::GetCurrentPlot()->Axes[ImAxis_X1].Range.Min < glob::game::BOARD_WIDTH.at(0)
        || ImPlot::GetCurrentPlot()->Axes[ImAxis_X1].Range.Max > glob::game::BOARD_WIDTH.at(1))
    {
        if (ImPlot::GetCurrentPlot()->Axes[ImAxis_Y1].Range.Min < glob::game::BOARD_HEIGHT.at(0)
            || ImPlot::GetCurrentPlot()->Axes[ImAxis_Y1].Range.Max > glob::game::BOARD_HEIGHT.at(1))
        {
            ImPlot::SetupAxesLimits(x_min, x_max, y_min, y_max, ImPlotCond_Always);
        }
        else
        {
            ImPlot::SetupAxisLimits(ImAxis_X1, x_min, x_max, ImPlotCond_Always);
        }
    }
    else if (ImPlot::GetCurrentPlot()->Axes[ImAxis_Y1].Range.Min < glob::game::BOARD_HEIGHT.at(0)
             || ImPlot::GetCurrentPlot()->Axes[ImAxis_Y1].Range.Max > glob::game::BOARD_HEIGHT.at(1))
    {
        ImPlot::SetupAxisLimits(ImAxis_Y1, y_min, y_max, ImPlotCond_Always);
    }
    else
    {
        ImPlot::SetupAxesLimits(glob::game::BOARD_WIDTH.at(0), glob::game::BOARD_WIDTH.at(1),
                                glob::game::BOARD_HEIGHT.at(0), glob::game::BOARD_HEIGHT.at(1),
                                ImPlotCond_Once);
    }
}

void GamePlot::Draw(float plotSize)
{
    if (ImPlot::BeginPlot("GamePlot", ImVec2(plotSize + 15, plotSize), ImPlotFlags_Equal | ImPlotFlags_NoTitle))
    {
        ScalePlotAxes();

        ImPlot::PushPlotClipRect();

        // This draws 4 transparent points into each corner so that double clicking the plot will zoom out
        constexpr std::array<std::array<double, 2>, 4> gameBoard = { {
            { glob::game::BOARD_WIDTH.at(0), glob::game::BOARD_HEIGHT.at(0) },
            { glob::game::BOARD_WIDTH.at(0), glob::game::BOARD_HEIGHT.at(1) },
            { glob::game::BOARD_WIDTH.at(1), glob::game::BOARD_HEIGHT.at(0) },
            { glob::game::BOARD_WIDTH.at(1), glob::game::BOARD_HEIGHT.at(1) },
        } };
        ImPlot::SetNextLineStyle(ImColor{ 0, 0, 0, 0 });
        ImPlot::PlotScatter("##Game board", gameBoard.at(0).data(), gameBoard.at(0).data() + 1, gameBoard.size());

        float logoSize = 18.0F;
        Eigen::Vector2f logoPos{ glob::game::BOARD_WIDTH.at(0) + 1, glob::game::BOARD_HEIGHT.at(0) + 1 };
        Eigen::Vector2f upperLeft = logoPos + Eigen::Vector2f{ 0.0F * logoSize, 1.0F * logoSize * 1484.0F / 2036.0F };
        Eigen::Vector2f lowerRright = logoPos + Eigen::Vector2f{ 1.0F * logoSize, 0.0F * logoSize };
        ImPlot::GetPlotDrawList()->AddImage(GameApplication::logoTexture,
                                            ImPlot::PlotToPixels(upperLeft.x(), upperLeft.y()),
                                            ImPlot::PlotToPixels(lowerRright.x(), lowerRright.y()),
                                            ImVec2(0, 0), ImVec2(1, 1), ImColor(1.0F, 1.0F, 1.0F, 0.6F));

        GameState::Draw();

        ImPlot::PopPlotClipRect();

        ImPlot::EndPlot();
    }
}

} // namespace oop::internal