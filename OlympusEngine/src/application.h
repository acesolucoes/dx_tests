#pragma once

#include "platform/WIN32/IApplication.h"

class Application : public IApplication {

    // Application

    public:
        Application();
        ~Application();

    public:

        void SetupPerGameSettings();
        void Initialize();
        void Update();
};