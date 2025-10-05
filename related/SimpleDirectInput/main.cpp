#include <windows.h>
#include <dinput.h>
#include <iostream>

LPDIRECTINPUT8 g_pDI = nullptr;
int g_controllerCount = 0;

BOOL CALLBACK EnumJoysticksCallback(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext) {
    std::cout << "Controller found: " << pdidInstance->tszProductName << std::endl;
    g_controllerCount++;
    return DIENUM_CONTINUE; // keep enumerating
}

int main() {
    // Create DirectInput interface
    if (FAILED(DirectInput8Create(GetModuleHandle(NULL),
                                  DIRECTINPUT_VERSION,
                                  IID_IDirectInput8,
                                  (VOID**)&g_pDI, NULL))) {
        std::cerr << "Failed to initialize DirectInput." << std::endl;
        return 1;
    }

    // Enumerate game controller devices
    if (FAILED(g_pDI->EnumDevices(DI8DEVCLASS_GAMECTRL,
                                  EnumJoysticksCallback,
                                  NULL, DIEDFL_ATTACHEDONLY))) {
        std::cerr << "Failed to enumerate devices." << std::endl;
        g_pDI->Release();
        return 1;
    }

    std::cout << "Total connected controllers: " << g_controllerCount << std::endl;

    g_pDI->Release();
    return 0;
}
