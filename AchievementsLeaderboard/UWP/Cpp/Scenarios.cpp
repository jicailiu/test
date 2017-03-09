////*********************************************************
////
//// Copyright (c) Microsoft. All rights reserved.
//// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
//// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
//// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
////
////*********************************************************
#include "pch.h"
#include "Scenarios.h"
#include "MainPage.xaml.h"

const WCHAR StatsNameEnemyDefeats[] = L"EnemyDefeats";
const WCHAR LeaderboardIdEnemyDefeats[] = L"LBEnemyDefeatsDescending";

std::mutex g_blockOfTextLock;

using namespace Sample;
using namespace pplx;

ScenarioCallback* Scenarios::m_scenarioCallbackClass;
std::shared_ptr<xbox::services::user_statistics::statistic_change_subscription> Scenarios::m_subscription;
MainPage^ Scenarios::m_mainPage;

void Scenarios::Scenario_GetAchievement(
    _In_ MainPage^ mainPage, 
    _In_ std::shared_ptr<xbox::services::xbox_live_context> xboxLiveContext
    )
{
    xboxLiveContext->achievement_service().get_achievement(
        xboxLiveContext->user()->xbox_user_id(),
        xboxLiveContext->application_config()->scid(),
        L"1")
    .then([this, mainPage](xbox::services::xbox_live_result<xbox::services::achievements::achievement> achievementResult )
    {
        if (achievementResult.err())
        {
            mainPage->LogFormat(L"get_achievement failed: %S", achievementResult.err_message().c_str());
        }
        else
        {
            auto achievement = achievementResult.payload();
            std::lock_guard<std::mutex> lockGuard(g_blockOfTextLock);
            mainPage->LogFormat(L"");
            mainPage->LogFormat(L"----------------");
            mainPage->LogFormat(L"get_achievement result:");
            LogAchievement(mainPage, achievement);
        }
    }, pplx::task_continuation_context::use_current());
}


void Scenarios::Scenario_GetSingleUserStatisticsAsync(
    _In_ MainPage^ mainPage, 
    _In_ std::shared_ptr<xbox::services::xbox_live_context> xboxLiveContext
    )
{
    xboxLiveContext->user_statistics_service().get_single_user_statistics(
        xboxLiveContext->user()->xbox_user_id(),
        xboxLiveContext->application_config()->scid(),
        L"EnemyDefeats"
        )
    .then([this, mainPage](xbox::services::xbox_live_result<xbox::services::user_statistics::user_statistics_result> resultDetails)
    {
        std::lock_guard<std::mutex> lockGuard(g_blockOfTextLock);
        if (resultDetails.err())
        {
            mainPage->Log(L"");
            mainPage->Log(L"----------------");
            mainPage->LogFormat(L"get_single_user_statistics failed: %S", resultDetails.err_message().c_str());
        }
        else
        {
            auto result = std::move(resultDetails.payload());
            LogUserStatisticsResult(mainPage, result, L"get_single_user_statistics");
        }
    }, pplx::task_continuation_context::use_current());
}

void Scenarios::Scenario_WriteEvent(
    _In_ MainPage^ mainPage, 
    _In_ std::shared_ptr<xbox::services::xbox_live_context> xboxLiveContext
    )
{
    // Write an event with data fields 
    web::json::value properties = web::json::value::object();
    properties[L"PlayerSessionId"] = web::json::value(L"session id");
    properties[L"MultiplayerCorrelationId"] = web::json::value(L"multiplayer correlation id");
    properties[L"GameplayModeId"] = web::json::value(L"gameplay mode id");
    properties[L"DifficultyLevelId"] = 100;
    properties[L"RoundId"] = 1;
    properties[L"PlayerRoleId"] = 1;
    properties[L"PlayerWeaponId"] = 2;
    properties[L"EnemyRoleId"] = 3;
    properties[L"KillTypeId"] = 4;

    web::json::value measurements = web::json::value::object();
    measurements[L"LocationX"] = 1;
    measurements[L"LocationY"] = 2.12121;
    measurements[L"LocationZ"] = -90909093;

    xbox::services::xbox_live_result<void> result = xboxLiveContext->events_service().write_in_game_event(L"EnemyDefeated", properties, measurements);

    mainPage->Log(L"");
    mainPage->Log(L"----------------");
    if (!result.err())
    {
        mainPage->Log(L"EnemyDefeated event was fired");
    }
    else
    {
        mainPage->Log(L"Failed to fire EnemyDefeated event");
    }
}

void Scenarios::Scenario_ActivateRTAAndStartStatSubscription(
    _In_ MainPage^ ui, 
    _In_ std::shared_ptr<xbox::services::xbox_live_context> xboxLiveContext
    )
{
    m_mainPage = ui;

    xboxLiveContext->real_time_activity_service()->activate();

    xboxLiveContext->user_statistics_service().add_statistic_changed_handler(
        std::bind(
            &ScenarioCallback::StatisticChanged,
            m_scenarioCallbackClass,
            std::placeholders::_1
            )
        );

    auto subscriptionResult = xboxLiveContext->user_statistics_service().subscribe_to_statistic_change(
        xboxLiveContext->user()->xbox_user_id(),
        xboxLiveContext->application_config()->scid(),
        StatsNameEnemyDefeats
        );

    std::lock_guard<std::mutex> lockGuard(g_blockOfTextLock);

    ui->Log(L"");
    ui->Log(L"----------------");
    if (!subscriptionResult.err())
    {
        m_subscription = subscriptionResult.payload();
        ui->Log(L"subscribe_to_statistic_change result:");
        ui->LogFormat(L"Statistic state: %d", m_subscription->state());
    }
    else
    {
        ui->LogFormat(L"subscribe_to_statistic_change error: %S", subscriptionResult.err_message().c_str());
    }
}

void ScenarioCallback::StatisticChanged(_In_ xbox::services::user_statistics::statistic_change_event_args args)
{
    Scenarios::m_mainPage->Dispatcher->RunAsync(
        Windows::UI::Core::CoreDispatcherPriority::Normal,
        ref new Windows::UI::Core::DispatchedHandler([this, args]()
    {
        std::lock_guard<std::mutex> lockGuard(g_blockOfTextLock);
        Scenarios::m_mainPage->Log(L"");
        Scenarios::m_mainPage->LogFormat(L"----------------");
        Scenarios::m_mainPage->LogFormat(L"statistic change:");
        Scenarios::m_mainPage->LogFormat(L"\tStatistic Name:%s", args.latest_statistic().statistic_name().c_str());
        Scenarios::m_mainPage->LogFormat(L"\tStatistic Type:%s", args.latest_statistic().statistic_type().c_str());
        Scenarios::m_mainPage->LogFormat(L"\tStatistic Value:%s", args.latest_statistic().value().c_str());
    }));
}

void Scenarios::Scenario_StopStatisticSubscription(
    _In_ MainPage^ ui, 
    _In_ std::shared_ptr<xbox::services::xbox_live_context> xboxLiveContext
    )
{
    auto unsubscribeResult = xboxLiveContext->user_statistics_service().unsubscribe_from_statistic_change(
        m_subscription
        );

    std::lock_guard<std::mutex> lockGuard(g_blockOfTextLock);

    ui->Log(L"");
    ui->Log(L"----------------");
    if (!unsubscribeResult.err())
    {
        ui->Log(L"Unsubscribed successfully");
    }
    else
    {
        ui->LogFormat(L"subscribe_to_statistic_change error: %S", unsubscribeResult.err_message().c_str());
    }
}

void Scenarios::Scenario_DeactivateRTA(
    _In_ MainPage^ ui, 
    _In_ std::shared_ptr<xbox::services::xbox_live_context> xboxLiveContext
    )
{
    xboxLiveContext->real_time_activity_service()->deactivate();

    ui->Log(L"");
    ui->Log(L"----------------");
    ui->Log(L"RTA deactivated");
}

void Scenarios::LogUserStatisticsResult(_In_ MainPage^ mainPage, _In_ xbox::services::user_statistics::user_statistics_result& t, _In_ const std::wstring& headerName)
{
    mainPage->Log(L"");
    mainPage->Log(L"----------------");
    mainPage->LogFormat(L"%s result:", headerName.c_str());

    mainPage->LogFormat(L"Xbox user ID:%s", t.xbox_user_id().c_str());
    for (const xbox::services::user_statistics::service_configuration_statistic& configStatistic : t.service_configuration_statistics())
    {
        mainPage->LogFormat(L"\tService Configuration ID:%s", configStatistic.service_configuration_id().c_str());
        for (const xbox::services::user_statistics::statistic& statistic : configStatistic.statistics())
        {
            mainPage->LogFormat(L"\t\tStatistic Name:%s", statistic.statistic_name().c_str());
            mainPage->LogFormat(L"\t\tStatistic Type:%s", statistic.statistic_type().c_str());
            mainPage->LogFormat(L"\t\tStatistic Value:%s", statistic.value().c_str());
        }
    }
}

void Scenarios::LogAchievement(
    _In_ MainPage^ mainPage,
    _In_ const xbox::services::achievements::achievement& achievement
    )
{
    bool unlocked = (achievement.progress_state() == xbox::services::achievements::achievement_progress_state::achieved);
    mainPage->LogFormat(L"  ----Achievement: '%s' (%d) ------", achievement.name().c_str(), achievement.id().c_str());
    mainPage->LogFormat(L"    Description: %s", unlocked ? achievement.unlocked_description().c_str() : achievement.locked_description().c_str());
    mainPage->LogFormat(L"    AchievementType: %d", static_cast<int>(achievement.type()));
    mainPage->LogFormat(L"    ProgressState: %d", static_cast<int>(achievement.progress_state()));
}

void Scenarios::ProcessLeaderboardResult(
    _In_ MainPage^ mainPage,
    _In_ task<xbox::services::xbox_live_result<xbox::services::leaderboard::leaderboard_result>> &t,
    _In_ const std::wstring& headerName
    )
{
    auto resultDetails = t.get();
    if (resultDetails.err())
    {
        std::lock_guard<std::mutex> lockGuard(g_blockOfTextLock);
        mainPage->Log(L"");
        mainPage->Log(L"----------------");
        mainPage->LogFormat(L"ProcessLeaderboardResult Error: %S", resultDetails.err_message().c_str());
        return;
    }

    auto result = resultDetails.payload();
    std::lock_guard<std::mutex> lockGuard(g_blockOfTextLock);
    LogLeaderboardResult(mainPage, result, headerName);

    create_async([this, result, mainPage, headerName]()
    {
        int pageCount = 5; // get next 5 pages 
        while (result.has_next() && pageCount-- > 0)
        {
            auto newRes = result;

            newRes.get_next(15)
            .then([this, mainPage, &newRes, headerName](xbox::services::xbox_live_result<xbox::services::leaderboard::leaderboard_result> leaderboardResult)
            {
                if (leaderboardResult.err())
                {
                    auto errMessage = leaderboardResult.err_message();
                    mainPage->Dispatcher->RunAsync(
                        Windows::UI::Core::CoreDispatcherPriority::Normal,
                        ref new Windows::UI::Core::DispatchedHandler([mainPage, leaderboardResult, errMessage]()
                    {
                        std::lock_guard<std::mutex> lockGuard(g_blockOfTextLock);
                        mainPage->Log(L"");
                        mainPage->Log(L"----------------");
                        mainPage->LogFormat(L"ProcessLeaderboardResult Get Next Error: %S", errMessage.c_str());
                    }));
                    return;
                }
                else
                {
                    newRes = std::move(leaderboardResult.payload());
                    mainPage->Dispatcher->RunAsync(
                        Windows::UI::Core::CoreDispatcherPriority::Normal,
                        ref new Windows::UI::Core::DispatchedHandler([this, mainPage, headerName, newRes]()
                    {
                        LogLeaderboardResult(mainPage, newRes, headerName + L" get_next");
                    }));
                }
            }).wait();
        }
    });
}

void Scenarios::LogLeaderboardResult(
    _In_ MainPage^ mainPage,
    _In_ const xbox::services::leaderboard::leaderboard_result &result,
    _In_ const std::wstring& headerName
    )
{
    mainPage->Log(L"");
    mainPage->Log(L"----------------");
    mainPage->LogFormat(L"%s result:", headerName.c_str());
    mainPage->LogFormat(L"Columns: %s", GetColumnNamesDisplayString(result).c_str());
    for (const xbox::services::leaderboard::leaderboard_row& row : result.rows())
    {
        mainPage->LogFormat(L"Gamertag: %s, Rank: %d, Percentile: %f, Values: %s",
            row.gamertag().c_str(),
            row.rank(),
            row.percentile(),
            GetRowValuesDisplayString(row).c_str()
            );
    }
}

std::wstring Scenarios::GetColumnNamesDisplayString(
    _In_ const xbox::services::leaderboard::leaderboard_result& result
    )
{
    std::wstringstream ss;
    auto &last = result.columns().back();
    for (const xbox::services::leaderboard::leaderboard_column& col : result.columns())
    {
        ss << col.display_name();
        if (&col != &last)
        {
            ss << L",";
        }
    }
    return ss.str();
}

std::wstring Scenarios::GetRowValuesDisplayString(
    _In_ const xbox::services::leaderboard::leaderboard_row& row
    )
{
    std::wstringstream ss;
    auto &last = row.column_values().back();
    for (const std::wstring rowValue : row.column_values())
    {
        ss << rowValue;
        if (&rowValue != &last)
        {
            ss << L",";
        }
    }
    return ss.str();
}
