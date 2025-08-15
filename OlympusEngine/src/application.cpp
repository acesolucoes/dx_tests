#include "pch.h"
#include "application.h"
#include "platform/WIN32/WinEntry.h"

ENTRYAPP(Application)

Application::Application() {
    //
}
        
Application::~Application() {
    //
}

VOID Application::SetupPerGameSettings() {
    /* Set the Per Game Settings */
    // PerGameSettings::SetGameName(IDS_PERGAMENAME);
    // PerGameSettings::SetShortName(IDS_SHORTNAME);
    // PerGameSettings::SetMainIcon(IDI_MAINICON);
    PerGameSettings::InitializeVariables();
}

void Application::Initialize() 
{
    Logger::printDebugSeparator();
    Logger::printLog(L"Application Starting...\n");
    Logger::printLog(L"Game Name: %s\n", PerGameSettings::GameName());
    Logger::printLog(L"Boot Time: %s\n", PerGameSettings::BootTime());
    Logger::printDebugSeparator();

    Logger::startMTail();
}

void Application::Update() {
}