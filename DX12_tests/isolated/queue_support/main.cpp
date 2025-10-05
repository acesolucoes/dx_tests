#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <iostream>
using Microsoft::WRL::ComPtr;

int main() {
    ComPtr<IDXGIFactory6> factory;
    CreateDXGIFactory1(IID_PPV_ARGS(&factory));

    ComPtr<IDXGIAdapter1> adapter;
    for (UINT i = 0;
         factory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND;
         ++i) 
    {
        DXGI_ADAPTER_DESC1 desc;
        adapter->GetDesc1(&desc);
        std::wcout << L"Adapter: " << desc.Description << std::endl;

        // Step 2: Create device
        ComPtr<ID3D12Device> device;
        if (SUCCEEDED(D3D12CreateDevice(adapter.Get(),
                                        D3D_FEATURE_LEVEL_11_0,
                                        IID_PPV_ARGS(&device)))) 
        {
            // Step 3: Check support for each queue type
            for (D3D12_COMMAND_LIST_TYPE type : {
                     D3D12_COMMAND_LIST_TYPE_DIRECT,
                     D3D12_COMMAND_LIST_TYPE_COMPUTE,
                     D3D12_COMMAND_LIST_TYPE_COPY
                 }) 
            {
                D3D12_COMMAND_QUEUE_DESC descQueue = {};
                descQueue.Type = type;
                descQueue.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

                ComPtr<ID3D12CommandQueue> queue;
                HRESULT hr = device->CreateCommandQueue(&descQueue,
                                                        IID_PPV_ARGS(&queue));
                if (SUCCEEDED(hr)) {
                    switch (type) {
                        case D3D12_COMMAND_LIST_TYPE_DIRECT:
                            std::cout << "  Supports DIRECT queue" << std::endl;
                            break;
                        case D3D12_COMMAND_LIST_TYPE_COMPUTE:
                            std::cout << "  Supports COMPUTE queue" << std::endl;
                            break;
                        case D3D12_COMMAND_LIST_TYPE_COPY:
                            std::cout << "  Supports COPY queue" << std::endl;
                            break;
                    }
                }
            }
        }
    }
}
