#include "GameState.hpp"

#include <spdlog/spdlog.h>
#include <fmt/core.h>
#include <implot.h>
#include <chrono>

#include "internal/helper/RandomNumberGenerator.hpp"
#include "helper/RandomNumber.hpp"
#include "internal/GameApplication.hpp"
#include "internal/game/Settings.hpp"
#include "internal/game/resources/Resource.hpp"
#include "internal/game/neutral/NeutralPlayer.hpp"

#include "TEAMNAME/units/Robot.hpp"
#include "TEAMNAME/units/Headquarters.hpp"
#include "TEAMNAME/player/Player.hpp"

namespace oop::internal
{

namespace hidden
{

size_t currentGid = 1;
} // namespace hidden

std::vector<std::shared_ptr<PlayerBase>> GameState::players;
std::vector<Resource> GameState::resources;
std::vector<Satellite> GameState::satellites;
std::variant<std::shared_ptr<const Unit>,
             const Resource*,
             const Satellite*>
    GameState::selectedObject;

std::vector<int> itemsCloseToPlayer;

size_t GameState::GetNextGID()
{
    return hidden::currentGid++;
}

Eigen::Vector2f GameState::GetNewResourcePosition(const Eigen::Vector2f& center, float maxDistance)
{
    Eigen::Vector2f pos;

    bool positionCloseToOthers = false;
    do
    {
        positionCloseToOthers = false;
        if (maxDistance == -1)
        {
            pos = Eigen::Vector2f{ RandomNumberGenerator::gameRngGenerator().uniform_real_distribution<double>(glob::game::BOARD_WIDTH.at(0) * 0.95,
                                                                                                               glob::game::BOARD_WIDTH.at(1) * 0.95),
                                   RandomNumberGenerator::gameRngGenerator().uniform_real_distribution<double>(glob::game::BOARD_HEIGHT.at(0) * 0.95,
                                                                                                               glob::game::BOARD_HEIGHT.at(1) * 0.95) };
        }
        else
        {
            auto heading = RandomNumberGenerator::gameRngGenerator().uniform_real_distribution<float>(0, 2.0 * static_cast<float>(M_PI));
            auto distance = RandomNumberGenerator::gameRngGenerator().uniform_real_distribution<float>(glob::resources::MIN_DISTANCE_RESOURCE_TO_HQ, maxDistance);
            pos = center + distance * Eigen::Vector2f{ std::cos(heading), std::sin(heading) };
            if (pos.x() < glob::game::BOARD_WIDTH.at(0) * 0.95
                || pos.x() > glob::game::BOARD_WIDTH.at(1) * 0.95
                || pos.y() < glob::game::BOARD_HEIGHT.at(0) * 0.95
                || pos.y() > glob::game::BOARD_HEIGHT.at(1) * 0.95)
            {
                positionCloseToOthers = true;
                continue;
            }
        }
        if (pos.x() < glob::game::BOARD_WIDTH.at(0) + 20 && pos.y() < glob::game::BOARD_HEIGHT.at(0) + 20) // Do not spawn resources inside the logo
        {
            positionCloseToOthers = true;
            continue;
        }

        for (size_t i = 1; i < players.size(); i++)
        {
            const auto& hq = players.at(i)->m_units.front();

            if ((pos - hq->m_pos).norm() <= glob::resources::MIN_DISTANCE_RESOURCE_TO_HQ_LIMITED
                && itemsCloseToPlayer.at(i - 1) >= glob::resources::RESOURCES_ALLOWED_CLOSE_TO_HQ)
            {
                positionCloseToOthers = true;
                break;
            }

            if ((pos - hq->m_pos).norm() <= glob::resources::MIN_DISTANCE_RESOURCE_TO_HQ)
            {
                positionCloseToOthers = true;
                break;
            }
        }
        if (positionCloseToOthers)
        {
            continue;
        }

        for (const auto& resource : resources)
        {
            if ((pos - resource.m_pos).norm() <= glob::resources::MIN_DISTANCE_RESOURCE_TO_RESOURCE)
            {
                positionCloseToOthers = true;
                break;
            }
        }

    } while (positionCloseToOthers);

    for (size_t i = 1; i < players.size(); i++)
    {
        const auto& hq = players.at(i)->m_units.front();
        if ((pos - hq->m_pos).norm() <= glob::resources::MIN_DISTANCE_RESOURCE_TO_HQ_LIMITED)
        {
            itemsCloseToPlayer.at(i - 1)++;
        }
    }

    return pos;
}

std::pair<Eigen::Vector2f, float> GameState::GetNewSatellitePositionAndHeading()
{
    auto heading = static_cast<float>(RandomNumberGenerator::gameRngGenerator().normal_distribution<double>(-M_PI_2, M_PI_2));
    auto startBorder = RandomNumberGenerator::gameRngGenerator().uniform_int_distribution<size_t>(0, 3);

    heading += static_cast<float>(startBorder) * static_cast<float>(M_PI_2);

    Eigen::Vector2f pos;
    if (startBorder == 0 || startBorder == 2) // Bottom || Top
    {
        auto center = (glob::game::BOARD_WIDTH.at(0) + glob::game::BOARD_WIDTH.at(1)) / 2.0;
        auto range = (glob::game::BOARD_WIDTH.at(0) - glob::game::BOARD_WIDTH.at(1)) / 2.0;
        pos = Eigen::Vector2f{ RandomNumberGenerator::gameRngGenerator().normal_distribution<>(center, 0.8 * range, glob::game::BOARD_WIDTH.at(0), glob::game::BOARD_WIDTH.at(1)),
                               glob::game::BOARD_HEIGHT.at(startBorder / 2) };
    }
    else if (startBorder == 1 || startBorder == 3) // Right || Left
    {
        auto center = (glob::game::BOARD_HEIGHT.at(0) + glob::game::BOARD_HEIGHT.at(1)) / 2.0;
        auto range = std::abs(glob::game::BOARD_HEIGHT.at(0) - glob::game::BOARD_HEIGHT.at(1)) / 2.0;
        pos = Eigen::Vector2f{ glob::game::BOARD_WIDTH.at(startBorder % 3),
                               RandomNumberGenerator::gameRngGenerator().normal_distribution<>(center, 0.8 * range, glob::game::BOARD_HEIGHT.at(0), glob::game::BOARD_HEIGHT.at(1)) };
    }

    return { pos, heading };
}

void GameState::OnStart()
{
    // ------------------------------------------------- Reset ---------------------------------------------------
    hidden::currentGid = 1;
    selectedObject.emplace<0>(nullptr);
    players.clear();
    resources.clear();
    satellites.clear();
    itemsCloseToPlayer.clear();

    RandomNumberGenerator::gameRngGenerator().reset();

    GameApplication::gameRunning = true; // This lets the normal user rng generator reset
    RandomNumberGenerator::userRngGenerator().useSeedInsteadOfSystemTime = true;
    RandomNumberGenerator::userRngGenerator().seed = RandomNumberGenerator::gameRngGenerator().uniform_int_distribution<uint64_t>();
    RandomNumberGenerator::userRngGenerator().reset();
    GameApplication::gameRunning = false; // This lets the pause user rng generator reset
    RandomNumberGenerator::userRngGenerator().useSeedInsteadOfSystemTime = true;
    RandomNumberGenerator::userRngGenerator().seed = RandomNumberGenerator::gameRngGenerator().uniform_int_distribution<uint64_t>();
    RandomNumberGenerator::userRngGenerator().reset();

    // ----------------------------------------------- Start new -------------------------------------------------

    std::array<float, ResourceType_COUNT> availableResources{};

    players.reserve(glob::game::NUM_PLAYERS + 1);

    // Neutral unit player
    players.push_back(std::make_shared<NeutralPlayer>(ImColor{ 224, 224, 224 }));

    for (size_t p = 0; p < glob::game::NUM_PLAYERS; p++)
    {
        auto startPosition = Eigen::Vector2f{ RandomNumberGenerator::gameRngGenerator().uniform_real_distribution<double>(glob::game::BOARD_WIDTH.at(0) * 0.9,
                                                                                                                          glob::game::BOARD_WIDTH.at(1) * 0.9),
                                              RandomNumberGenerator::gameRngGenerator().uniform_real_distribution<double>(glob::game::BOARD_HEIGHT.at(0) * 0.9,
                                                                                                                          glob::game::BOARD_HEIGHT.at(1) * 0.9) };
        // Create Player
        players.push_back(std::make_shared<TEAMNAME::Player>(startPosition, ImPlot::GetColormapColor(5 - static_cast<int>(p), ImPlot::GetColormapIndex("Dark"))));
        itemsCloseToPlayer.push_back(0); // Tracks resource positions relative to player

        // Spawn Headquarters
        auto hqHeading = RandomNumberGenerator::gameRngGenerator().uniform_real_distribution<float>(0, 2.0F * static_cast<float>(M_PI));
        players.back()->AddUnit(std::make_shared<TEAMNAME::Headquarters>(players.back().get(), GetNextGID(), startPosition, hqHeading));
        if (players.back()->m_units.size() > 1) // HQ constructor adds units so the HQ actually gets added last
        {
            std::swap(players.back()->m_units.front(), players.back()->m_units.back());
        }

        for (ResourceType t = 0; t < ResourceType_COUNT; ++t)
        {
            // Calculate total available resources on the map
            availableResources.at(t) = RandomNumberGenerator::gameRngGenerator().uniform_real_distribution<float>(glob::resources::AMOUNT_RESOURCES_TOTAL.at(t).front(),
                                                                                                                  glob::resources::AMOUNT_RESOURCES_TOTAL.at(t).back());

            // Spawn resource in close proximity to headquaters
            auto amount = RandomNumberGenerator::gameRngGenerator().uniform_real_distribution<float>(glob::resources::AMOUNT_RESOURCES_PER_ENTITY.at(t).front(),
                                                                                                     glob::resources::AMOUNT_RESOURCES_PER_ENTITY.at(t).back());
            resources.emplace_back(t, amount, GetNewResourcePosition(startPosition, glob::resources::MIN_DISTANCE_RESOURCE_TO_HQ_LIMITED), M_PI / 180 * 0.0F);
            availableResources.at(t) -= amount;
        }
    }

    // Spawn resources till the total available amount is exhausted
    while (std::any_of(availableResources.begin(), availableResources.end(), [](float amount) { return amount > 0; }))
    {
        for (ResourceType t = 0; t < ResourceType_COUNT; t++)
        {
            if (availableResources.at(t) > 0)
            {
                auto amount = RandomNumberGenerator::gameRngGenerator().uniform_real_distribution<float>(glob::resources::AMOUNT_RESOURCES_PER_ENTITY.at(t).front(),
                                                                                                         glob::resources::AMOUNT_RESOURCES_PER_ENTITY.at(t).back());
                resources.emplace_back(t, amount, GetNewResourcePosition(), 0.0F);
                availableResources.at(t) -= amount;
            }
        }
    }
}

void GameState::Update(float deltaTime)
{
    while (satellites.size() < glob::positioning::NUM_SAT)
    {
        auto [pos, heading] = GetNewSatellitePositionAndHeading();
        satellites.emplace_back(pos, heading);
    }

    if (GameApplication::gameRunning)
    {
        for (auto& satellite : satellites)
        {
            satellite.Update(deltaTime);
        }
    }

    for (const auto& player : players)
    {
        if (GameApplication::gameRunning)
        {
            player->Think(deltaTime);
        }
        for (const auto& unit : player->m_units)
        {
            unit->UpdateAlways();
            if (GameApplication::gameRunning && player->m_isAlive)
            {
                unit->Update(deltaTime);
            }
        }
    }

    // --------------------------------------- Cleanup obsolete objects ------------------------------------------
    for (auto& player : players)
    {
        for (auto unitIter = player->m_units.cbegin(); unitIter != player->m_units.cend(); unitIter++)
        {
            if ((*unitIter)->m_currentHealth <= 0)
            {
                if (std::holds_alternative<std::shared_ptr<const Unit>>(selectedObject)
                    && std::get<std::shared_ptr<const Unit>>(selectedObject)
                    && std::get<std::shared_ptr<const Unit>>(selectedObject)->m_gid == (*unitIter)->m_gid)
                {
                    std::get<std::shared_ptr<const Unit>>(selectedObject) = nullptr;
                }

                if (unitIter == player->m_units.cbegin() // HQ
                    && player->m_gid != 0)               // and not neutral player
                {
                    player->m_isAlive = false;
                }

                auto eraseIter = unitIter;
                unitIter--;
                player->m_units.erase(eraseIter);
            }
        }
    }

    for (auto resIter = resources.cbegin(); resIter != resources.cend(); resIter++)
    {
        if (resIter->m_amount == 0)
        {
            if (std::holds_alternative<const Resource*>(selectedObject)
                && std::get<const Resource*>(selectedObject)
                && std::get<const Resource*>(selectedObject)->m_gid == resIter->m_gid)
            {
                std::get<const Resource*>(selectedObject) = nullptr;
            }

            auto eraseIter = resIter;
            resIter--;
            resources.erase(eraseIter);
        }
    }

    for (auto satIter = satellites.cbegin(); satIter != satellites.cend(); satIter++)
    {
        if (satIter->m_pos.x() < glob::game::BOARD_WIDTH.at(0) || satIter->m_pos.x() > glob::game::BOARD_WIDTH.at(1)
            || satIter->m_pos.y() < glob::game::BOARD_HEIGHT.at(0) || satIter->m_pos.y() > glob::game::BOARD_HEIGHT.at(1))
        {
            if (std::holds_alternative<const Satellite*>(selectedObject)
                && std::get<const Satellite*>(selectedObject)
                && std::get<const Satellite*>(selectedObject)->m_gid == satIter->m_gid)
            {
                std::get<const Satellite*>(selectedObject) = nullptr;
            }

            auto eraseIter = satIter;
            satIter--;
            satellites.erase(eraseIter);
        }
    }

    // --------------------------------------------- Win condition -----------------------------------------------
    if (!GameApplication::gameFinished)
    {
        if (resources.empty()) // All resources collected
        {
            bool unitCarryingResources = false;
            for (const auto& player : players)
            {
                for (const auto& unit : player->m_units)
                {
                    if (unit->m_resourcesCarried.second)
                    {
                        unitCarryingResources = true;
                        break;
                    }
                }
                if (unitCarryingResources)
                {
                    break;
                }
            }
            if (!unitCarryingResources) // No unit carrying resources
            {
                GameApplication::gameRunning = false;
                GameApplication::gameFinished = 2;

                size_t winningPlayerIndex = 0;
                size_t winningResSum = 0;
                for (size_t p = 1; p < players.size(); ++p)
                {
                    size_t resSum = 0;
                    for (ResourceType resType = 0; resType < ResourceType_COUNT; ++resType)
                    {
                        resSum += players.at(p)->m_collectedResourcesTotal.at(resType);
                    }
                    if (resSum > winningResSum)
                    {
                        winningResSum = resSum;
                        winningPlayerIndex = p;
                    }
                }
                GameApplication::winningPlayerColor = players.at(winningPlayerIndex)->GetColor();
            }
        }

        size_t playersAlive = 0;
        for (const auto& player : players)
        {
            if (!player->m_gid) // Neutral player
            {
                continue;
            }
            if (size_t unitCnt = player->m_units.size() - 1; // Player has only one unit left (has to be HQ, otherwise already dead)
                unitCnt == 0)
            {
                for (ResourceType resType = 0; resType < ResourceType_COUNT; ++resType) // Not enough resources to build new unit
                {
                    if (player->m_resources.at(resType) < static_cast<size_t>(glob::units::ROBOT_COSTS.at(resType)))
                    {
                        player->m_isAlive = false;
                        break;
                    }
                }
            }

            if (player->m_isAlive)
            {
                playersAlive += 1;
            }
        }
        if (playersAlive == 0) // Single player (game lost)
        {
            GameApplication::gameRunning = false;
            GameApplication::gameFinished = 2;
        }
        else if (glob::game::ENABLE_PVP                                // PVP enabled
                 && players.size() > 2                                 // Multiplayer
                 && (playersAlive == 1 && !glob::game::NEUTRAL_UNITS)) // Only one player alive and no neutral units spawned
        {
            GameApplication::gameRunning = false;
            GameApplication::gameFinished = 2;
            for (const auto& player : players)
            {
                if (!player->m_gid) // Neutral player
                {
                    continue;
                }
                if (player->m_isAlive)
                {
                    GameApplication::winningPlayerColor = player->GetColor();
                }
            }
        }
    }
}

void GameState::Draw()
{
    bool somethingSelected = false;
    for (const auto& player : players)
    {
        for (const auto& unit : player->m_units)
        {
            if (ImPlot::IsPlotHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) // Click on unit
            {
                Eigen::Vector2f mousePos{ ImPlot::GetPlotMousePos().x, ImPlot::GetPlotMousePos().y };
                if ((unit->m_pos - mousePos).norm() <= unit->GetDrawSize() * glob::gui::HOVER_OBJECT_SIZE_MODIFIER)
                {
                    selectedObject = unit;
                    somethingSelected = true;
                }
            }
            unit->Draw();
        }
    }

    for (const auto& resource : resources)
    {
        if (ImPlot::IsPlotHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) // Click on unit
        {
            Eigen::Vector2f mousePos{ ImPlot::GetPlotMousePos().x, ImPlot::GetPlotMousePos().y };
            if ((resource.m_pos - mousePos).norm() <= resource.GetDrawSize() * glob::gui::HOVER_OBJECT_SIZE_MODIFIER)
            {
                selectedObject = &resource;
                somethingSelected = true;
            }
        }

        resource.Draw();
    }

    for (const auto& satellite : satellites)
    {
        if (ImPlot::IsPlotHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) // Click on unit
        {
            Eigen::Vector2f mousePos{ ImPlot::GetPlotMousePos().x, ImPlot::GetPlotMousePos().y };
            if ((satellite.m_pos - mousePos).norm() <= Satellite::m_size * glob::gui::HOVER_OBJECT_SIZE_MODIFIER)
            {
                selectedObject = &satellite;
                somethingSelected = true;
            }
        }

        satellite.Draw();
    }

    if (!somethingSelected && ImPlot::IsPlotHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) // Click into empty space
    {
        selectedObject.emplace<0>(nullptr);
    }
}

void GameState::DrawGameStats(float availableWidth)
{
    constexpr float heightPlayer = 150.0F;
    constexpr float heightPlayerVirus = 75.0F;
    float heightTooltip = 0;

    if (std::holds_alternative<std::shared_ptr<const Unit>>(selectedObject)
        && std::get<std::shared_ptr<const Unit>>(selectedObject))
    {
        const auto& unit = std::get<std::shared_ptr<const Unit>>(selectedObject);
        if (unit->m_parent->m_gid) // Player
        {
            if (unit->IsHeadquarters())
            {
                heightTooltip = 210;
            }
            else
            {
                heightTooltip = 320;
            }
        }
        else // Neutral
        {
            heightTooltip = 235;
        }
    }

    else if (std::holds_alternative<const Resource*>(selectedObject)
             && std::get<const Resource*>(selectedObject))
    {
        heightTooltip = 90;
    }
    else if (std::holds_alternative<const Satellite*>(selectedObject)
             && std::get<const Satellite*>(selectedObject))
    {
        const auto& satellite = std::get<const Satellite*>(selectedObject);
        heightTooltip = satellite->m_isFaulty ? 180 : 140;
    }

    float heightStats = 55 // Elapsed Time
                        + ImGui::GetStyle().WindowPadding.y
                        + (glob::game::NEUTRAL_UNITS ? heightPlayerVirus : 0)
                        + ImGui::GetStyle().WindowPadding.y
                        + heightPlayer * glob::game::NUM_PLAYERS
                        + (heightTooltip > 0 ? ImGui::GetStyle().WindowPadding.y / 2.0F + heightTooltip : 0.0F);

    ImGui::BeginChild("ControlPanel Stats", ImVec2(availableWidth, heightStats), true); // 750

    int gameTimeHours = static_cast<int>(GameApplication::gameTime / 3600.0F);
    int gameTimeMin = static_cast<int>(GameApplication::gameTime / 60) - gameTimeHours * 60;
    int gameTimeSec = static_cast<int>(GameApplication::gameTime) - gameTimeHours * 3600 - gameTimeMin * 60;
    int gameTimeMs = static_cast<int>((GameApplication::gameTime - std::floor(GameApplication::gameTime)) * 1e3F);

    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[3]);
    ImGui::TextUnformatted(fmt::format("Elapsed Time {:02d}:{:02d}:{:02d}.{:03d}", gameTimeHours, gameTimeMin, gameTimeSec, gameTimeMs).c_str());
    ImGui::PopFont();

    // ###########################################################################################################
    //                                               Player Stats
    // ###########################################################################################################

    for (auto& player : players)
    {
        if (!player->m_gid && !glob::game::NEUTRAL_UNITS)
        {
            continue;
        }

        ImGui::BeginChild(fmt::format("ControlPanel Stats Player {}", player->m_gid).c_str(),
                          ImVec2(availableWidth - 2.0F * ImGui::GetStyle().WindowPadding.x, player->m_gid ? heightPlayer : heightPlayerVirus), true);

        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[2]);
        ImGui::TextUnformatted(player->m_gid ? "Player" : "Virus");
        ImGui::PopFont();
        ImGui::SameLine();
        ImGui::ColorButton(fmt::format("##player color {}", player->m_gid).c_str(), player->m_color.Value, ImGuiColorEditFlags_NoTooltip, ImVec2(30, 30));

        if (ImGui::BeginTable("##player stats", 2, ImGuiTableFlags_None))
        {
            ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 50.0F);

            ImGui::TableNextColumn();
            ImGui::TextUnformatted("Units");
            ImGui::TableNextColumn();
            ImGui::Text("%lu", player->m_units.size() - (player->m_gid ? 1 : 0));

            // TODO: Killed count
            // TODO: Lost count

            ImGui::EndTable();
        }

        if (player->m_gid)
        {
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0F);

            if (ImGui::BeginTable(fmt::format("##player resources table {}", player->m_gid).c_str(), 4, ImGuiTableFlags_Borders))
            {
                ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 50.0F);
                ImGui::TableSetupColumn("Capacitors");
                ImGui::TableSetupColumn("Coils");
                ImGui::TableSetupColumn("Resistors");
                ImGui::TableHeadersRow();

                ImGui::TableNextColumn();
                ImGui::TextUnformatted("Current");

                for (size_t n = 0; n < ResourceType_COUNT; n++)
                {
                    ImGui::TableNextColumn();
                    ImGui::TextUnformatted(fmt::format("{}", player->m_resources.at(n)).c_str());
                }

                ImGui::TableNextColumn();
                ImGui::TextUnformatted("Total");
                for (size_t n = 0; n < ResourceType_COUNT; n++)
                {
                    ImGui::TableNextColumn();
                    ImGui::TextUnformatted(fmt::format("{}", player->m_collectedResourcesTotal.at(n)).c_str());
                }
                ImGui::EndTable();
            }
        }

        ImGui::EndChild();
    }

    // ###########################################################################################################
    //                                                  Tooltip
    // ###########################################################################################################
    if (std::holds_alternative<std::shared_ptr<const Unit>>(selectedObject)
        && std::get<std::shared_ptr<const Unit>>(selectedObject))
    {
        const auto& unit = std::get<std::shared_ptr<const Unit>>(selectedObject);

        ImGui::BeginChild("ControlPanel Selected Object",
                          ImVec2(availableWidth - 2.0F * ImGui::GetStyle().WindowPadding.x, heightTooltip), true);

        bool isHQ = unit->IsHeadquarters();

        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[2]);
        ImGui::TextColored(unit->m_parent->GetColor(), "%s", fmt::format("[{}] {}", unit->m_gid, unit->GetUnitTypeName()).c_str());
        ImGui::PopFont();

        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4{ 1.0F - unit->m_currentHealth / unit->m_maxHealth, unit->m_currentHealth / unit->m_maxHealth, 0.0F, 1.0F });
        ImGui::ProgressBar(unit->m_currentHealth / unit->m_maxHealth, ImVec2(availableWidth - 4.0F * ImGui::GetStyle().WindowPadding.x, 0),
                           fmt::format("Health: {:.1f} / {:.1f}", unit->m_currentHealth, unit->m_maxHealth).c_str());
        ImGui::SameLine();
        ImGui::TextUnformatted("Health");
        ImGui::PopStyleColor();

        if (ImGui::BeginTable("##unit stats", 2, ImGuiTableFlags_None))
        {
            auto actionToString = [](Unit::Action action) {
                switch (action)
                {
                case oop::internal::Unit::Action_None:
                    return "Nothing";
                case oop::internal::Unit::Action_Move:
                    return "Moving";
                case oop::internal::Unit::Action_Attack:
                    return "Attacking";
                case oop::internal::Unit::Action_CollectResource:
                    return "Collecting resources";
                case oop::internal::Unit::Action_DropOffResourcesAtHQ:
                    return "Dropping resources off at HQ";
                case oop::internal::Unit::Action_DiscardResources:
                    return "Discarding resources";
                }
                return "Nothing";
            };

            ImGui::TableNextColumn();
            ImGui::TextUnformatted("Action");
            ImGui::TableNextColumn();
            ImGui::Text("%s", actionToString(unit->m_actionTooltip));

            ImGui::TableNextColumn();
            ImGui::TextUnformatted("Position");
            ImGui::TableNextColumn();
            ImGui::Text("%.2f | %.2f", unit->m_pos.x(), unit->m_pos.y());

            ImGui::TableNextColumn();
            ImGui::TextUnformatted("Heading");
            ImGui::TableNextColumn();
            if (glob::game::ENABLE_HEADING_PRECISION)
            {
                ImGui::Text("%.1f° ± %.1f°", unit->m_heading * 180.0F / static_cast<float>(M_PI), unit->m_headingBias * 180.0F / static_cast<float>(M_PI));
            }
            else
            {
                ImGui::Text("%.1f°", unit->m_heading * 180.0F / static_cast<float>(M_PI));
            }

            if (!isHQ && unit->m_parent->m_gid)
            {
                ImGui::TableNextColumn();
                ImGui::TextUnformatted("Carried Resources");
                ImGui::TableNextColumn();
                if (unit->m_resourcesCarried.second)
                {
                    ImGui::Text("%s - %lu", Resource::GetTypeName(unit->m_resourcesCarried.first).c_str(), unit->m_resourcesCarried.second);
                }
                else
                {
                    ImGui::TextUnformatted("None");
                }

                ImGui::TableNextColumn();
                ImGui::TextUnformatted("Satellites");
                ImGui::TableNextColumn();
                ImGui::Text("%zu", unit->m_satelliteCount);
            }

            ImGui::TableNextRow();
            ImGui::TableNextRow();

            if (!isHQ)
            {
                ImGui::TableNextColumn();
                ImGui::TextUnformatted("Speed");
                ImGui::TableNextColumn();
                ImGui::Text("%.1f", unit->m_speed - (unit->m_resourcesCarried.second ? glob::units::SPEED_DECREASE_WHILE_CARRYING : 0.0F));
            }

            ImGui::TableNextColumn();
            ImGui::TextUnformatted("Scan range");
            ImGui::TableNextColumn();
            ImGui::Text("%.1f", unit->m_scanRange);

            if (!isHQ && unit->m_parent->m_gid)
            {
                ImGui::TableNextColumn();
                ImGui::TextUnformatted("Collect range");
                ImGui::TableNextColumn();
                ImGui::Text("%.1f", unit->m_collectRange);

                ImGui::TableNextColumn();
                ImGui::TextUnformatted("Container size");
                ImGui::TableNextColumn();
                ImGui::Text("%d", unit->m_resourceContainerSize);
            }

            ImGui::TableNextColumn();
            ImGui::TextUnformatted("Attack power");
            ImGui::TableNextColumn();
            ImGui::Text("%d", unit->m_attackPower);

            ImGui::TableNextColumn();
            ImGui::TextUnformatted("Attack range");
            ImGui::TableNextColumn();
            ImGui::Text("%.1f", unit->m_attackRange);

            if (!isHQ && glob::game::ENABLE_DISTANCE_CLOCK_OFFSET)
            {
                ImGui::TableNextColumn();
                ImGui::TextUnformatted("Clock offset");
                ImGui::TableNextColumn();
                ImGui::Text("%.3e", unit->m_clockOffset);
            }

            ImGui::EndTable();
        }

        ImGui::EndChild();
    }
    else if (std::holds_alternative<const Resource*>(selectedObject)
             && std::get<const Resource*>(selectedObject))
    {
        ImGui::BeginChild("ControlPanel Selected Object", ImVec2(availableWidth - 2.0F * ImGui::GetStyle().WindowPadding.x, heightTooltip), true);

        const auto& resource = std::get<const Resource*>(selectedObject);
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[2]);
        ImGui::TextColored(resource->color(), "%s", fmt::format("[{}] {}", resource->m_gid, resource->GetTypeName()).c_str());
        ImGui::PopFont();

        if (ImGui::BeginTable("##resource stats", 2, ImGuiTableFlags_None))
        {
            ImGui::TableNextColumn();
            ImGui::TextUnformatted("Position");
            ImGui::TableNextColumn();
            ImGui::Text("%.2f | %.2f", resource->m_pos.x(), resource->m_pos.y());

            ImGui::TableNextColumn();
            ImGui::TextUnformatted("Amount");
            ImGui::TableNextColumn();
            ImGui::Text("%d", resource->m_amount);

            ImGui::EndTable();
        }

        ImGui::EndChild();
    }
    else if (std::holds_alternative<const Satellite*>(selectedObject)
             && std::get<const Satellite*>(selectedObject))
    {
        ImGui::BeginChild("ControlPanel Selected Object", ImVec2(availableWidth - 2.0F * ImGui::GetStyle().WindowPadding.x, heightTooltip), true);

        const auto& satellite = std::get<const Satellite*>(selectedObject);
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[2]);
        ImGui::TextColored(Satellite::m_color, "[%zu] Satellite", satellite->m_gid);
        ImGui::PopFont();

        if (ImGui::BeginTable("##satellite stats", 2, ImGuiTableFlags_None))
        {
            ImGui::TableNextColumn();
            ImGui::TextUnformatted("Position");
            ImGui::TableNextColumn();
            ImGui::Text("%.2f | %.2f", satellite->m_pos.x(), satellite->m_pos.y());

            ImGui::TableNextColumn();
            ImGui::TextUnformatted("Speed");
            ImGui::TableNextColumn();
            ImGui::Text("%.1f", satellite->m_speed);

            ImGui::TableNextColumn();
            ImGui::TextUnformatted("Heading");
            ImGui::TableNextColumn();
            ImGui::Text("%.1f°", satellite->m_heading * 180.0F / static_cast<float>(M_PI));

            ImGui::TableNextColumn();
            ImGui::TextUnformatted("Faulty");
            ImGui::TableNextColumn();
            ImGui::TextUnformatted(satellite->m_isFaulty ? "true" : "false");

            if (satellite->m_isFaulty)
            {
                ImGui::TableNextColumn();
                ImGui::Indent();
                ImGui::TextUnformatted("Position");
                ImGui::Unindent();
                ImGui::TableNextColumn();
                ImGui::Text("%.2f | %.2f", satellite->m_faultyPos.x(), satellite->m_faultyPos.y());

                ImGui::TableNextColumn();
                ImGui::Indent();
                ImGui::TextUnformatted("Speed");
                ImGui::Unindent();
                ImGui::TableNextColumn();
                ImGui::Text("%.1f", satellite->m_faultySpeed);
            }

            ImGui::EndTable();
        }

        ImGui::EndChild();
    }

    // ImGui::TextUnformatted(fmt::format("Elapsed Time {:02d}:{:02d}:{:02d}.{:03d}", gameTimeHours, gameTimeMin, gameTimeSec, gameTimeMs).c_str());

    ImGui::EndChild();
}

} // namespace oop::internal