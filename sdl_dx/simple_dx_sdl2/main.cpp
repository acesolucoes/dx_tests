#include <SDL.h>
#include <SDL_syswm.h>
#include <d3d11.h>
#include <dxgi.h>
#include <iostream>

int main(int argc, char* argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Create an SDL window
    SDL_Window* window = SDL_CreateWindow("SDL DirectX Window",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          800, 600,
                                          SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    if (!window) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // Retrieve the native window handle (HWND) from the SDL window
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(window, &wmInfo);
    HWND hwnd = wmInfo.info.win.window;
    // Describe the swap chain
    DXGI_SWAP_CHAIN_DESC scd = {};
    scd.BufferCount = 1;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow = hwnd;
    scd.SampleDesc.Count = 1;
    scd.Windowed = TRUE;

    // Create the Direct3D device and swap chain
    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* context = nullptr;
    IDXGISwapChain* swapChain = nullptr;

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr,                    // Adapter (nullptr = use default)
        D3D_DRIVER_TYPE_HARDWARE,   // Driver Type
        nullptr,                    // Software
        0,                          // Flags
        nullptr,                    // Feature Levels
        0,                          // Feature Levels Count
        D3D11_SDK_VERSION,          // SDK Version
        &scd,                       // Swap Chain Description
        &swapChain,                 // Swap Chain
        &device,                    // Device
        nullptr,                    // Feature Level
        &context                    // Device Context
    );

    if (FAILED(hr)) {
        std::cerr << "Failed to create Direct3D device and swap chain" << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    // Main loop
    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        // Rendering code goes here
        // ...

        // Present the swap chain
        swapChain->Present(1, 0);
    }

    // Cleanup
    if (swapChain) swapChain->Release();
    if (context) context->Release();
    if (device) device->Release();
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
