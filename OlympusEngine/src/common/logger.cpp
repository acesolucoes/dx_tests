#include "olympus.h"
#include <fstream>
#include <shlobj_core.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <objbase.h>
#include <cstdio>
#include <tlhelp32.h>

Logger* Logger::inst;

Logger::Logger() {
    inst = this;
}

Logger::~Logger() {
    // 
}

void Logger::printLog( const WCHAR* fmt, ... ) {
    WCHAR buf[4096];
    va_list args;

    va_start(args, fmt);
    vswprintf_s(buf, fmt, args);
    va_end(args);

    //MessageBox(0, buf, 0, 0);
    OutputDebugString(buf);

    std::wfstream outfile;
    outfile.open(std::wstring(logDirectory() + L"/" + logFile()), std::ios_base::app);
    
    if( outfile.is_open() ) {
        std::wstring s = buf;
        outfile << L"[" << Time::getDateTimeString() << L"] " << s;
        outfile.close();
        OutputDebugString(s.c_str());
    }
    else {
        MessageBox(0, L"Unable to open file...", L"Log Error", MB_OK);
    }
    
}

/* Get and Create Log Directory*/
std::wstring Logger::logDirectory()
{
    WCHAR Path[1024];
    WCHAR* AppDataLocal;
    SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &AppDataLocal);
    wcscpy_s(Path, AppDataLocal);
    wcscat_s(Path, L"\\");
    wcscat_s(Path, PerGameSettings::GameName());
    CreateDirectory(Path, NULL);
    wcscat_s(Path, L"\\Log");
    CreateDirectory(Path, NULL);
    return Path;
}

std::wstring Logger::logFile()
{
    WCHAR File[1024];
    wcscpy_s(File, PerGameSettings::GameName());
    wcscat_s(File, PerGameSettings::BootTime());
    wcscat_s(File, L".Log");
    return File;
}

/* Print a separator line without time stamp */
VOID Logger::printDebugSeparator() 
{
    std::wstring s = L"\n---------------------------------------------------------\n\n";
    
    #ifdef _DEBUG
    std::wfstream outfile;
    outfile.open(std::wstring(logDirectory() + L"/" + logFile()), std::ios_base::app);

    if (outfile.is_open()) {
        outfile << s;
        outfile.close();
    } else {
        MessageBox(0, L"Unable to open file...", L"Log Error", MB_OK);
    }
    #endif
}

/* Private class to check to see if MTail is already running - So we don't open multiple copies during debug */
BOOL Logger::isMTailRunning()
{
    bool exists = false;
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

    if (Process32First(snapshot, &entry))
        while (Process32Next(snapshot, &entry))
            if (!_wcsicmp(entry.szExeFile, L"mTAIL.exe"))
                exists = true;
    
    CloseHandle(snapshot);
    return exists;
}

/* Start MTail from Project or Build Directory - Depends on where ran from */
VOID Logger::startMTail()
{
    if (isMTailRunning()) {
        Logger::printLog(L"--MTail failed to start - ALready Running");
    }

    Logger::printLog(L"--Starting MTail\n");
    //std::wstring url = FileIO::GetWorkingDir().c_str() + std::wstring(L"/mTAIL.exe");
    WCHAR path[MAX_PATH] = {0};
    GetCurrentDirectoryW(MAX_PATH, path);
    std::wstring url = path + std::wstring(L"/mTAIL.exe");
    std::wstring params = L" \"" + logDirectory() + L"/" + logFile() + L"\" /start";
    ShellExecute(0, NULL, url.c_str(), params.c_str(), NULL, SW_SHOWDEFAULT);
}