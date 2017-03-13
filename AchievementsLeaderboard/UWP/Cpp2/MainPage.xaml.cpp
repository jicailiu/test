//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#include "pch.h"
#include "MainPage.xaml.h"
#include "xsapi\services.h"

using namespace concurrency;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Sample;

struct ScenarioDescriptionItem
{
    int tag;
    Platform::String^ name;
};

enum ScenarioItemTag
{
    Scenario_GetAchievement,
    Scenario_GetAchievementsForTitleId,
    Scenario_GetLeaderboardAsync,
    Scenario_GetLeaderboardSkipToRankAsync,
    Scenario_GetLeaderboardSkipToXuidAsync,
    Scenario_GetLeaderboardForSocialGroupAsync,
    Scenario_GetLeaderboardForSocialGroupWithSortAsync,
    Scenario_GetLeaderboardForSocialGroupSkipToRankAsync,
    Scenario_GetLeaderboardForSocialGroupSkipToXuidAsync,
    Scenario_GetSingleUserStatisticsAsync,
    Scenario_GetMultipleUserStatisticsAsync,
    Scenario_WriteEvent,
    Scenario_ActivateRTAAndStartStatSubscription,
    Scenario_StopStatisticSubscription,
    Scenario_DeactivateRTA
};

ScenarioDescriptionItem ScenarioDescriptions[] =
{
    //{ Scenario_GetAchievement, "Get Achievement" },
    { Scenario_GetAchievementsForTitleId, "Get Achievements" },
    //{ Scenario_GetLeaderboardAsync, "Get leaderboard" },
    //{ Scenario_GetLeaderboardSkipToRankAsync, "Get leaderboard skip to rank #2" },
    //{ Scenario_GetLeaderboardSkipToXuidAsync, "Get leaderboard skip to self" },
    //{ Scenario_GetLeaderboardForSocialGroupAsync, "Get leaderboard of friends" },
    //{ Scenario_GetLeaderboardForSocialGroupWithSortAsync, "Get leaderboard of friends and sorts" },
    //{ Scenario_GetLeaderboardForSocialGroupSkipToRankAsync, "Get leaderboard of friends skip to rank" },
    //{ Scenario_GetLeaderboardForSocialGroupSkipToXuidAsync, "Get leaderboard of friends skip to self" },
    { Scenario_GetSingleUserStatisticsAsync, "Get single user statistic" },
    //{ Scenario_GetMultipleUserStatisticsAsync, "Get multiple users statistic" },
    { Scenario_WriteEvent, "Write Events" },
    //{ Scenario_ActivateRTAAndStartStatSubscription, "Activate RTA and Start Statistic Subscription" },
    //{ Scenario_StopStatisticSubscription, "Stop Statistic Subscription" },
    //{ Scenario_DeactivateRTA, "Deactivate RTA" }
};

bool MainPage::RunScenario(int selectedTag)
{
    if (m_xboxLiveContext == nullptr || !m_xboxLiveContext->user()->is_signed_in())
    {
        Log(L"No user signed in");
        return false;
    }

    switch (selectedTag)
    {
        case Scenario_GetAchievement: m_scenarios.Scenario_GetAchievement(this, m_xboxLiveContext); break;
        case Scenario_GetAchievementsForTitleId: m_scenarios.Scenario_GetAchievementsForTitleId(this, m_xboxLiveContext); break;
        case Scenario_GetLeaderboardAsync: m_scenarios.Scenario_GetLeaderboardAsync(this, m_xboxLiveContext); break;
        case Scenario_GetLeaderboardSkipToRankAsync: m_scenarios.Scenario_GetLeaderboardForSocialGroupSkipToRankAsync(this, m_xboxLiveContext); break;
        case Scenario_GetLeaderboardSkipToXuidAsync: m_scenarios.Scenario_GetLeaderboardForSocialGroupSkipToXuidAsync(this, m_xboxLiveContext); break;
        case Scenario_GetLeaderboardForSocialGroupAsync: m_scenarios.Scenario_GetLeaderboardForSocialGroupAsync(this, m_xboxLiveContext); break;
        case Scenario_GetLeaderboardForSocialGroupWithSortAsync: m_scenarios.Scenario_GetLeaderboardForSocialGroupWithSortAsync(this, m_xboxLiveContext); break;
        case Scenario_GetLeaderboardForSocialGroupSkipToRankAsync: m_scenarios.Scenario_GetLeaderboardForSocialGroupSkipToRankAsync(this, m_xboxLiveContext); break;
        case Scenario_GetLeaderboardForSocialGroupSkipToXuidAsync: m_scenarios.Scenario_GetLeaderboardForSocialGroupSkipToXuidAsync(this, m_xboxLiveContext); break;
        case Scenario_GetSingleUserStatisticsAsync: m_scenarios.Scenario_GetSingleUserStatisticsAsync(this, m_xboxLiveContext); break;
        case Scenario_GetMultipleUserStatisticsAsync: m_scenarios.Scenario_GetMultipleUserStatisticsAsync(this, m_xboxLiveContext); break;
        case Scenario_WriteEvent: m_scenarios.Scenario_WriteEvent(this, m_xboxLiveContext); break;
        case Scenario_ActivateRTAAndStartStatSubscription: m_scenarios.Scenario_ActivateRTAAndStartStatSubscription(this, m_xboxLiveContext); break;
        case Scenario_StopStatisticSubscription: m_scenarios.Scenario_StopStatisticSubscription(this, m_xboxLiveContext); break;
        case Scenario_DeactivateRTA: m_scenarios.Scenario_DeactivateRTA(this, m_xboxLiveContext); break;
        default: return false;
    }

    return true;
}

MainPage::MainPage()
{
    InitializeComponent();
    this->CoreDispatcher = Windows::UI::Xaml::Window::Current->CoreWindow->Dispatcher;

    for (ScenarioDescriptionItem scenario : ScenarioDescriptions)
    {
        ListBoxItem^ listBoxItem = ref new ListBoxItem();
        listBoxItem->Content = scenario.name;
        listBoxItem->Tag = (Platform::Object^)scenario.tag;
        this->ScenarioListBox->Items->Append(listBoxItem);
    }

    this->ScenarioListBox->SelectedIndex = 0;
    m_user = std::make_shared< xbox::services::system::xbox_live_user >();
    m_user->add_sign_out_completed_handler([this](const xbox::services::system::sign_out_completed_event_args& args)
    {
        // dispaly log on the UI thread
        Dispatcher->RunAsync(
            Windows::UI::Core::CoreDispatcherPriority::Normal,
            ref new Windows::UI::Core::DispatchedHandler([this, args]()
        {
            this->UserInfoLabel->Text = L"user signed out";

            Log(L"----------------");
            LogFormat(L"User %s signed out", args.user()->gamertag());

        }));
    });

    SignIn();
}

void MainPage::ScenarioListBox_DoubleTapped(
    Platform::Object^ sender, 
    Windows::UI::Xaml::Input::DoubleTappedRoutedEventArgs^ e
    )
{
    RunSelectedScenario();
}

void MainPage::ClearLogs()
{
    this->OutputStackPanel->Children->Clear();
}

void MainPage::LogFormat(LPCWSTR strMsg, ...)
{
    WCHAR strBuffer[2048];

    va_list args;
    va_start(args, strMsg);
    _vsnwprintf_s(strBuffer, 2048, _TRUNCATE, strMsg, args);
    strBuffer[2047] = L'\0';

    va_end(args);

    Platform::String^ str = ref new Platform::String(strBuffer);
    Log(str);
}

void MainPage::Log(Platform::String^ logLine)
{
    TextBlock^ uiElement = ref new TextBlock();
    uiElement->FontSize = 14;
    uiElement->Text = logLine;
    this->OutputStackPanel->Children->Append(uiElement);
}

void MainPage::RunButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    RunSelectedScenario();
}

void MainPage::RunSelectedScenario()
{
    if (this->ScenarioListBox->SelectedItems->Size == 1)
    {
        ClearLogs();
        ListBoxItem^ selectedItem = safe_cast<ListBoxItem^>(this->ScenarioListBox->SelectedItem);
        int selectedTag = safe_cast<int>(selectedItem->Tag);
        RunScenario(selectedTag);
    }
}

void MainPage::RunAllButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    ClearLogs();
    int scenarioTag = 1;
    for (;;)
    {
        bool success = RunScenario(scenarioTag);
        if (!success)
        {
            break;
        }

        scenarioTag++;
    }
}


void MainPage::SignInButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    SignIn();
}

void MainPage::SignInSilently()
{
    this->UserInfoLabel->Text = L"Trying to sign in silently...";
    Log(this->UserInfoLabel->Text);

    m_user->signin_silently()
    .then([this](xbox::services::xbox_live_result<xbox::services::system::sign_in_result> t) // use task_continuation_context::use_current() to make the continuation task running in current apartment 
    {
        if (!t.err())
        {
            auto result = t.payload();
            switch (result.status())
            {
            case xbox::services::system::sign_in_status::success:
                OnSignInSucceeded();
                break;
            case xbox::services::system::sign_in_status::user_interaction_required:
                this->UserInfoLabel->Text = L"user_interaction_required";
                Log(L"user_interaction_required");
                break;
            }
        }
        else
        {
            this->UserInfoLabel->Text = L"Sign in silently failed";
            Log(L"signin_silently failed.");
            string_t utf16Error = utility::conversions::utf8_to_utf16(t.err_message());
            Log(ref new Platform::String(utf16Error.c_str()));
        }
    }, task_continuation_context::use_current());
}

void MainPage::SignIn()
{
    this->UserInfoLabel->Text = L"Trying to sign in...";
    Log(this->UserInfoLabel->Text);

    m_user->signin(this->CoreDispatcher)
    .then([this](xbox::services::xbox_live_result<xbox::services::system::sign_in_result> t) // use task_continuation_context::use_current() to make the continuation task running in current apartment 
    {
        if (!t.err())
        {
            auto result = t.payload();
            switch (result.status())
            {
            case xbox::services::system::sign_in_status::success:
                OnSignInSucceeded();
                break;
            case xbox::services::system::sign_in_status::user_cancel:
                this->UserInfoLabel->Text = L"user_cancel";
                Log(L"user_cancel");
                break;
            }
        }
        else 
        {
            this->UserInfoLabel->Text = L"Sign in failed";
            Log(L"SignInAsync failed.");
            string_t utf16Error = utility::conversions::utf8_to_utf16(t.err_message());
            Log(ref new Platform::String(utf16Error.c_str()));
        }

    }, task_continuation_context::use_current());
}


void MainPage::OnSignInSucceeded()
{
    m_xboxLiveContext = std::make_shared< xbox::services::xbox_live_context >(m_user);

    UserInfoLabel->Text = L"Sign in succeeded";
    Log(this->UserInfoLabel->Text);
}
