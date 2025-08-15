#pragma once

#ifdef _WIN32
    #include <windows.h>
#endif

#ifdef BUILD_DLL
    #define OLYMPUS_API _declspec(dllexport)
#else
    #define OLYMPUS_API _declspec(dllimport)
#endif

#define MAX_NAME_STRING 256
#define HInstance() GetModuleHandle(NULL)

#include "common/logger.h"
#include "common/time.h"
#include "core/perGameSettings.h"