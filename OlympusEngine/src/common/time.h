#pragma once 
#include <string>

namespace Time {
    /* Get current time in string format */
    std::wstring OLYMPUS_API getTime(BOOL stripped = FALSE);
    
    /* Get current date in string format */
    std::wstring OLYMPUS_API getDate(BOOL stripped = FALSE);
    
    /* Get current date and time in string format */
    std::wstring OLYMPUS_API getDateTimeString(BOOL stripped = FALSE);
}