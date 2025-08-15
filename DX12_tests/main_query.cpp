#include <windows.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl.h>
#include <iostream>

using namespace Microsoft::WRL;

void QueryDX12Adapters() {
    // Create a DXGI factory
    ComPtr<IDXGIFactory4> factory;
    HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&factory));
    if (FAILED(hr)) {
        std::cerr << "Failed to create DXGI Factory. HRESULT: " << std::hex << hr << "\n";
        return;
    }

    // Enumerate adapters
    ComPtr<IDXGIAdapter1> adapter;
    UINT adapterIndex = 0;

    std::cout << "Available Direct3D 12 Adapters:\n";

    while (factory->EnumAdapters1(adapterIndex, &adapter) != DXGI_ERROR_NOT_FOUND) {
        DXGI_ADAPTER_DESC1 desc;
        adapter->GetDesc1(&desc);

        // Print adapter information
        std::wcout << L"Adapter " << adapterIndex << L": " << desc.Description << L"\n";

        // Check if the adapter supports Direct3D 12
        if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
            std::wcout << L"  (Software Adapter)\n";
        } else {
            ComPtr<ID3D12Device> device;
            if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device)))) {
                std::wcout << L"  Supports Direct3D 12\n";
            } else {
                std::wcout << L"  Does not support Direct3D 12\n";
            }
        }

        adapterIndex++;
    }

    if (adapterIndex == 0) {
        std::cout << "No adapters found.\n";
    }
}

int main() {
    QueryDX12Adapters();
    return 0;
}
