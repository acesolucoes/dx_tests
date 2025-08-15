#include "olympus.h"
#include <ctime>
#include <sstream>
#include <iomanip>

/* Get Time in format '00:00:00'  */
/* Stripped = 000000 */
std::wstring Time::getTime(BOOL stripped)
{
    time_t now = time(0);
    tm ltm;
    localtime_s(&ltm, &now);
    std::wstringstream wss;
    wss << std::put_time(&ltm, L"%T");

    std::wstring timeString = wss.str();

    if (stripped) {
        std::wstring chars = L":";
        for (WCHAR c : chars) {
            timeString.erase(std::remove(timeString.begin(), timeString.end(), c), timeString.end());
        }
    }
    return timeString;
}

/* Get Date in format '00/00/00'  */
/* Stripped = 000000 */
std::wstring Time::getDate(BOOL stripped)
{
    time_t now = time(0);
    tm ltm;
    localtime_s(&ltm, &now);
    std::wstringstream wss;
    wss << std::put_time(&ltm, L"%d/%m/%Y");

    std::wstring timeString = wss.str();

    if (stripped) {
        std::wstring chars = L"/";
        for (WCHAR c : chars) {
            timeString.erase(std::remove(timeString.begin(), timeString.end(), c), timeString.end());
        }
    }
    return timeString;
}

/* Get date time in format '00/00/00 00:00:00'  */
/* Stripped = 000000000000 */
std::wstring Time::getDateTimeString(BOOL stripped)
{
    std::wstring timeString = getDate(stripped) + L" " + getTime(stripped);

    if (stripped) {
        std::wstring chars = L" ";
        for (WCHAR c : chars) {
            timeString.erase(std::remove(timeString.begin(), timeString.end(), c), timeString.end());
        }
    }
    return timeString;
}