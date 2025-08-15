#include <d3d12.h>
#include <dxgi1_4.h>
#include <d3dcompiler.h>
#include <iostream>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")  // Optional for shader compilation

int main() {
    HRESULT hr;
    ID3D12Device* pDevice = nullptr;

    // Create DXGI Factory
    IDXGIFactory4* pFactory = nullptr;
    hr = CreateDXGIFactory1(IID_PPV_ARGS(&pFactory));
    if (FAILED(hr)) {
        std::cerr << "Failed to create DXGI Factory" << std::endl;
        return -1;
    }

    // Create D3D12 Device
    hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&pDevice));
    if (FAILED(hr)) {
        std::cerr << "Failed to create D3D12 Device" << std::endl;
        pFactory->Release();
        return -1;
    }

    // Define a root signature
    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
    rootSignatureDesc.NumParameters = 0;
    rootSignatureDesc.pParameters = nullptr;
    rootSignatureDesc.NumStaticSamplers = 0;
    rootSignatureDesc.pStaticSamplers = nullptr;
    rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    // Serialize the root signature
    ID3DBlob* pSignatureBlob = nullptr;
    ID3DBlob* pErrorBlob = nullptr;
    hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pSignatureBlob, &pErrorBlob);
    if (FAILED(hr)) {
        std::cerr << "Failed to serialize root signature" << std::endl;
        if (pErrorBlob) pErrorBlob->Release();
        pFactory->Release();
        pDevice->Release();
        return -1;
    }

    // Create the root signature
    ID3D12RootSignature* pRootSignature = nullptr;
    hr = pDevice->CreateRootSignature(0, pSignatureBlob->GetBufferPointer(), pSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&pRootSignature));
    if (FAILED(hr)) {
        std::cerr << "Failed to create root signature" << std::endl;
        pSignatureBlob->Release();
        pFactory->Release();
        pDevice->Release();
        return -1;
    }

    std::cout << "Root signature created successfully" << std::endl;

    // Clean up
    pRootSignature->Release();
    pSignatureBlob->Release();
    pFactory->Release();
    pDevice->Release();

    return 0;
}
