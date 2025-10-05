#include <windows.h>
#include <Xinput.h>
#include <iostream>

int main() {
    DWORD dwResult;
    int connected = 0;

    for (DWORD i = 0; i < XUSER_MAX_COUNT; i++) {
        XINPUT_STATE state;
        ZeroMemory(&state, sizeof(XINPUT_STATE));

        dwResult = XInputGetState(i, &state);
        if (dwResult == ERROR_SUCCESS) {
            std::cout << "Controller " << i << " is connected.\n";
            connected++;
        }
    }

    std::cout << "Total connected controllers: " << connected << std::endl;
    return 0;
}
