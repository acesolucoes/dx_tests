#include "olympus.h"

PerGameSettings* PerGameSettings::inst;

PerGameSettings::PerGameSettings() 
{
    inst = this;

    wcscpy_s(inst->m_GameName, L"undefined");
    wcscpy_s(inst->m_ShortName, L"undefined");
    wcscpy_s(inst->m_BootTime, Time::getDateTimeString(TRUE).c_str());
}

PerGameSettings::~PerGameSettings()
{
}

void PerGameSettings::InitializeVariables() {
    wcscpy_s(inst->m_GameName, L"Simple Game");
    wcscpy_s(inst->m_ShortName, L"SGame");
    //wcscpy_s(inst->m_BootTime, L"00:00");
}