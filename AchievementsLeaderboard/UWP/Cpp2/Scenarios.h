//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#pragma once

#include "MainPage.g.h"

namespace Sample
{
    class ScenarioCallback
    {
    public:
        ScenarioCallback() = default;
        void StatisticChanged(_In_ xbox::services::user_statistics::statistic_change_event_args args);
    };

    class Scenarios
    {
    public:
        void Scenario_GetAchievement(_In_ MainPage^ mainPage, _In_ std::shared_ptr<xbox::services::xbox_live_context> xboxLiveContext);
        void Scenario_GetAchievementsForTitleId(_In_ MainPage^ mainPage, _In_ std::shared_ptr<xbox::services::xbox_live_context> xboxLiveContext);
        void Scenario_GetLeaderboardAsync(_In_ MainPage^ mainPage, _In_ std::shared_ptr<xbox::services::xbox_live_context> xboxLiveContext);
        void Scenario_GetLeaderboardSkipToRankAsync(_In_ MainPage^ mainPage, _In_ std::shared_ptr<xbox::services::xbox_live_context> xboxLiveContext);
        void Scenario_GetLeaderboardSkipToXuidAsync(_In_ MainPage^ mainPage, _In_ std::shared_ptr<xbox::services::xbox_live_context> xboxLiveContext);
        void Scenario_GetLeaderboardForSocialGroupAsync(_In_ MainPage^ ui, _In_ std::shared_ptr<xbox::services::xbox_live_context> xboxLiveContext);
        void Scenario_GetLeaderboardForSocialGroupWithSortAsync(_In_ MainPage^ ui, _In_ std::shared_ptr<xbox::services::xbox_live_context> xboxLiveContext);
        void Scenario_GetLeaderboardForSocialGroupSkipToRankAsync(_In_ MainPage^ mainPage, _In_ std::shared_ptr<xbox::services::xbox_live_context> xboxLiveContext);
        void Scenario_GetLeaderboardForSocialGroupSkipToXuidAsync(_In_ MainPage^ mainPage, _In_ std::shared_ptr<xbox::services::xbox_live_context> xboxLiveContext);
        void Scenario_GetSingleUserStatisticsAsync(_In_ MainPage^ mainPage, _In_ std::shared_ptr<xbox::services::xbox_live_context> xboxLiveContext);
        void Scenario_GetMultipleUserStatisticsAsync(_In_ MainPage^ mainPage, _In_ std::shared_ptr<xbox::services::xbox_live_context> xboxLiveContext);
        void Scenario_WriteEvent(_In_ MainPage^ mainPage, _In_ std::shared_ptr<xbox::services::xbox_live_context> xboxLiveContext);
        void Scenario_ActivateRTAAndStartStatSubscription(_In_ MainPage^ ui, _In_ std::shared_ptr<xbox::services::xbox_live_context> xboxLiveContext);
        void Scenario_StopStatisticSubscription(_In_ MainPage^ ui, _In_ std::shared_ptr<xbox::services::xbox_live_context> xboxLiveContext);
        void Scenario_DeactivateRTA(_In_ MainPage^ ui, _In_ std::shared_ptr<xbox::services::xbox_live_context> xboxLiveContext);

        static MainPage^ m_mainPage;
    private:
        void LogUserStatisticsResult(_In_ MainPage^ mainPage, _In_ xbox::services::user_statistics::user_statistics_result& t, _In_ const std::wstring& headerName);
        void LogAchievement(_In_ MainPage^ mainPage, _In_ const xbox::services::achievements::achievement& achievement);
        void ProcessLeaderboardResult(
            _In_ MainPage^ mainPage,
            _In_ pplx::task<xbox::services::xbox_live_result<xbox::services::leaderboard::leaderboard_result>> &t,
            _In_ const std::wstring& headerName
            );

        void LogLeaderboardResult(
            _In_ MainPage^ mainPage,
            _In_ const xbox::services::leaderboard::leaderboard_result &result,
            _In_ const std::wstring& headerName
            );

        std::wstring GetColumnNamesDisplayString(
            _In_ const xbox::services::leaderboard::leaderboard_result& result
            );

        std::wstring GetRowValuesDisplayString(
            _In_ const xbox::services::leaderboard::leaderboard_row& row
            );

        static std::shared_ptr<xbox::services::user_statistics::statistic_change_subscription> m_subscription;
        static ScenarioCallback* m_scenarioCallbackClass;
    };
}
