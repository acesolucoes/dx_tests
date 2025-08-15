#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <windows.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl.h>
#include <iostream>

using namespace Microsoft::WRL;

void error_callback(int error, const char* description) {
    std::cerr << "GLFW Error: " << description << "\n";
}

int main() {
    // Initialize GLFW
    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    // Configure GLFW for no API (we'll use Direct3D 12)
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    // Create a window
    GLFWwindow* window = glfwCreateWindow(800, 600, "GLFW with Direct3D 12", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    // Get the native window handle
#ifdef _WIN32
    HWND hwnd = glfwGetWin32Window(window);
#else
    std::cerr << "Direct3D 12 is only supported on Windows\n";
    glfwDestroyWindow(window);
    glfwTerminate();
    return -1;
#endif

    // Initialize Direct3D 12
    ComPtr<ID3D12Device> device;
    ComPtr<IDXGIFactory4> factory;
    ComPtr<ID3D12CommandQueue> commandQueue;
    ComPtr<IDXGISwapChain3> swapChain;

    HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&factory));
    if (FAILED(hr)) {
        std::cerr << "Failed to create DXGI factory\n";
        return -1;
    }

    ComPtr<IDXGIAdapter1> adapter;
    factory->EnumAdapters1(0, &adapter);

    hr = D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));
    if (FAILED(hr)) {
        std::cerr << "Failed to create Direct3D 12 device\n";
        return -1;
    }

    // Create a command queue
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

    hr = device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue));
    if (FAILED(hr)) {
        std::cerr << "Failed to create command queue\n";
        return -1;
    }

    // Create a swap chain
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.BufferCount = 2;
    swapChainDesc.Width = 800;
    swapChainDesc.Height = 600;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.SampleDesc.Count = 1;

    ComPtr<IDXGISwapChain1> tempSwapChain;
    hr = factory->CreateSwapChainForHwnd(
        commandQueue.Get(),
        hwnd,
        &swapChainDesc,
        nullptr,
        nullptr,
        &tempSwapChain
    );

    if (FAILED(hr)) {
        std::cerr << "Failed to create swap chain\n";
        return -1;
    }

    hr = tempSwapChain.As(&swapChain);
    if (FAILED(hr)) {
        std::cerr << "Failed to cast swap chain\n";
        return -1;
    }

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Rendering would go here...
        // For now, we just present the swap chain
        swapChain->Present(1, 0);
    }

    // Cleanup
    swapChain.Reset();
    commandQueue.Reset();
    device.Reset();
    factory.Reset();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
