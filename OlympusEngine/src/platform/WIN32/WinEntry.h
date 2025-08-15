#include "olympus.h"

#include "IApplication.h"

extern IApplication* EntryApplication();

int CALLBACK WinMain(HINSTANCE, HINSTANCE, LPSTR, INT)
{
    auto entry = EntryApplication();

    PerGameSettings GameSettings;
    entry->SetupPerGameSettings();

    Logger logger;
    entry->Initialize();

    MSG msg = { 0 };
    while( msg.message != WM_QUIT )
    {
        // If there are WIndow messages then process them.
        if( PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } else {
            entry->Update();
        }
    }

    return 0;
}