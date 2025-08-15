#include <windows.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <d3dcompiler.h>
#include <stdio.h>

// Link against the necessary libraries
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")  // Optional

int main() {
    HRESULT hr;

    // Enable the D3D12 debug layer.
    ID3D12Debug* pDebugController;
    if (SUCCEEDED(D3D12GetDebugInterface(&IID_ID3D12Debug, (void**)&pDebugController))) {
        pDebugController->lpVtbl->EnableDebugLayer(pDebugController);
    }

    // Create the DXGI factory
    IDXGIFactory4* pFactory;
    hr = CreateDXGIFactory1(&IID_IDXGIFactory4, (void**)&pFactory);
    if (FAILED(hr)) {
        printf("Failed to create DXGI Factory\n");
        return -1;
    }

    // Create the D3D12 device
    ID3D12Device* pDevice;
    hr = D3D12CreateDevice(NULL, D3D_FEATURE_LEVEL_11_0, &IID_ID3D12Device, (void**)&pDevice);
    if (FAILED(hr)) {
        printf("Failed to create D3D12 Device\n");
        pFactory->lpVtbl->Release(pFactory);
        return -1;
    }

    // Define a root signature
    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {0};
    rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    // Serialize the root signature
    ID3DBlob* pSignatureBlob;
    ID3DBlob* pErrorBlob;
    hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pSignatureBlob, &pErrorBlob);
    if (FAILED(hr)) {
        printf("Failed to serialize root signature\n");
        if (pErrorBlob) pErrorBlob->lpVtbl->Release(pErrorBlob);
        pFactory->lpVtbl->Release(pFactory);
        pDevice->lpVtbl->Release(pDevice);
        return -1;
    }

    // Create the root signature
    ID3D12RootSignature* pRootSignature;
    hr = pDevice->lpVtbl->CreateRootSignature(pDevice, 0, pSignatureBlob->lpVtbl->GetBufferPointer(pSignatureBlob), pSignatureBlob->lpVtbl->GetBufferSize(pSignatureBlob), &IID_ID3D12RootSignature, (void**)&pRootSignature);
    if (FAILED(hr)) {
        printf("Failed to create root signature\n");
        pSignatureBlob->lpVtbl->Release(pSignatureBlob);
        pFactory->lpVtbl->Release(pFactory);
        pDevice->lpVtbl->Release(pDevice);
        return -1;
    }

    printf("Root signature created successfully\n");

    // Clean up
    pRootSignature->lpVtbl->Release(pRootSignature);
    pSignatureBlob->lpVtbl->Release(pSignatureBlob);
    if (pErrorBlob) pErrorBlob->lpVtbl->Release(pErrorBlob);
    pFactory->lpVtbl->Release(pFactory);
    pDevice->lpVtbl->Release(pDevice);

    return 0;
}
