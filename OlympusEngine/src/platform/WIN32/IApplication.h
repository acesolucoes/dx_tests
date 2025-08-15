#pragma once

#define ENTRYAPP(x) IApplication* EntryApplication() { return new x; }

class OLYMPUS_API IApplication {

    // Application

    public:
        IApplication();
        ~IApplication() {};

    public:
        /* Called to setup our pergame settings */
        virtual void SetupPerGameSettings() = 0;
        virtual void Initialize() = 0;
        virtual void Update() = 0;
};

IApplication* EntryApplication();