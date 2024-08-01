#pragma once

#include "Unity/IUnityGraphics.h"

#define _WIN32_WINNT 0x600
#include <stdio.h>
#include <stddef.h>
#include <iostream>

#include <d3d11.h>
#include <d3dcompiler.h>

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p)=nullptr; } }
#endif

struct IUnityInterfaces;

struct BufType
{
    float x, y, z, w;  
};

// The number of elements in a buffer to be tested
const UINT NUM_ELEMENTS = 1;

class D3D11Render {
private:
    ID3D11Device* m_Device;
    ID3D11DeviceContext* g_pContext = nullptr;
    ID3D11ComputeShader* g_computeShader = nullptr;

    ID3D11Buffer*               g_pBuf0 = nullptr;
    ID3D11Buffer*               g_pBufResult = nullptr;

    ID3D11ShaderResourceView*   g_pBuf0SRV = nullptr;
    ID3D11UnorderedAccessView*  g_pBufResultUAV = nullptr;

    std::ofstream &m_logger;
    bool confirmed = false;

public:
    D3D11Render(std::ofstream &output);

    void ProcessDeviceEvent(UnityGfxDeviceEventType type, IUnityInterfaces* interfaces);
    void setComputeBuffer(void *buffer);
    bool checkIfRanSuccessfully();
    bool checkSuccess(){ return confirmed; }

private:   
    void createResources();
    void releaseResources();
};