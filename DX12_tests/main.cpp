#include <windows.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl.h>
#include <iostream>

using namespace Microsoft::WRL;

void CreateAndShareBuffer() {
    // Initialize COM
    CoInitialize(nullptr);

    // Create two devices
    ComPtr<IDXGIFactory4> factory;
    HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&factory));
    if (FAILED(hr)) {
        std::cerr << "Failed to create DXGI factory\n";
        return;
    }

    ComPtr<IDXGIAdapter1> adapter1, adapter2;
    factory->EnumAdapters1(0, &adapter1);
    factory->EnumAdapters1(1, &adapter2); // Assuming at least two adapters

    ComPtr<ID3D12Device> device1, device2;
    D3D12CreateDevice(adapter1.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device1));
    D3D12CreateDevice(adapter2.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device2));

    // Describe and create a shared buffer
    D3D12_HEAP_PROPERTIES heapProps = {};
    heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
    heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    heapProps.CreationNodeMask = 1;
    heapProps.VisibleNodeMask = 1;

    D3D12_RESOURCE_DESC bufferDesc = {};
    bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    bufferDesc.Alignment = 0;
    bufferDesc.Width = 1024; // Buffer size
    bufferDesc.Height = 1;
    bufferDesc.DepthOrArraySize = 1;
    bufferDesc.MipLevels = 1;
    bufferDesc.Format = DXGI_FORMAT_UNKNOWN;
    bufferDesc.SampleDesc.Count = 1;
    bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    bufferDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_CROSS_ADAPTER;

    ComPtr<ID3D12Resource> buffer1;
    hr = device1->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_SHARED,
        &bufferDesc,
        D3D12_RESOURCE_STATE_COMMON,
        nullptr,
        IID_PPV_ARGS(&buffer1)
    );

    if (FAILED(hr)) {
        std::cerr << "Failed to create buffer on Device 1\n";
        return;
    }

    // Create a shared handle
    HANDLE sharedHandle = nullptr;
    hr = device1->CreateSharedHandle(buffer1.Get(), nullptr, GENERIC_ALL, nullptr, &sharedHandle);
    if (FAILED(hr)) {
        std::cerr << "Failed to create shared handle\n";
        return;
    }

    // Open the shared buffer on the second device
    ComPtr<ID3D12Resource> buffer2;
    hr = device2->OpenSharedHandle(sharedHandle, IID_PPV_ARGS(&buffer2));
    if (FAILED(hr)) {
        std::cerr << "Failed to open shared handle on Device 2\n";
        CloseHandle(sharedHandle);
        return;
    }

    std::cout << "Shared buffer created and opened successfully\n";

    // Clean up
    CloseHandle(sharedHandle);
    CoUninitialize();
}

int main() {
    CreateAndShareBuffer();
    return 0;
}
