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
        void Scenario_GetSingleUserStatisticsAsync(_In_ MainPage^ mainPage, _In_ std::shared_ptr<xbox::services::xbox_live_context> xboxLiveContext);
        void Scenario_WriteEvent(_In_ MainPage^ mainPage, _In_ std::shared_ptr<xbox::services::xbox_live_context> xboxLiveContext);
        void Scenario_ActivateRTAAndStartStatSubscription(_In_ MainPage^ ui, _In_ std::shared_ptr<xbox::services::xbox_live_context> xboxLiveContext);
        void Scenario_StopStatisticSubscription(_In_ MainPage^ ui, _In_ std::shared_ptr<xbox::services::xbox_live_context> xboxLiveContext);
        void Scenario_DeactivateRTA(_In_ MainPage^ ui, _In_ std::shared_ptr<xbox::services::xbox_live_context> xboxLiveContext);

        static MainPage^ m_mainPage;
    private:
        void LogUserStatisticsResult(_In_ MainPage^ mainPage, _In_ xbox::services::user_statistics::user_statistics_result& t, _In_ const std::wstring& headerName);
        void LogAchievement(_In_ MainPage^ mainPage, _In_ const xbox::services::achievements::achievement& achievement);

        static std::shared_ptr<xbox::services::user_statistics::statistic_change_subscription> m_subscription;
        static ScenarioCallback* m_scenarioCallbackClass;
    };
}
