#include <dxgi.h>
#include <dxgi1_2.h>
#include <d3d11.h>
#include <iostream>

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p)=nullptr; } }
#endif

void EnumerateDXGIDevices()
{
    IDXGIFactory1 *pFactory = nullptr;
    HRESULT hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&pFactory);

    if(FAILED(hr)) 
    {
        std::cerr << "Failed to create DXGIFactory1: " << std::hex << hr << std::hex;
        return;
    }

    UINT i = 0;
    IDXGIAdapter1* pAdapter = nullptr;
    while( pFactory->EnumAdapters1(i, &pAdapter) != DXGI_ERROR_NOT_FOUND )
    {
        DXGI_ADAPTER_DESC1 desc;
        pAdapter->GetDesc1(&desc);

        std::wcout << L"Adapter " << i << L": " << desc.Description << std::endl;

        UINT j = 0;
        IDXGIOutput* pOutput = nullptr;
        while( pAdapter->EnumOutputs(j, &pOutput) != DXGI_ERROR_NOT_FOUND )
        {
            DXGI_OUTPUT_DESC desc;
            pOutput->GetDesc(&desc);

            std::wcout << L"   Output " << j << L": " << desc.DeviceName << std::endl;

            pOutput->Release();
            ++j;
        }

        pAdapter->Release();
        ++i;
    }

    pFactory->Release();
}

int main() {
    EnumerateDXGIDevices();
    return 0;
}