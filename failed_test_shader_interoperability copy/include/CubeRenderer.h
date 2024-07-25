// CubeRenderer.h

#pragma once

#include <d3d11.h>
#include <directxmath.h>

using namespace DirectX;

class CubeRenderer {
public:
    CubeRenderer(HWND hwnd, int width, int height);
    ~CubeRenderer();
    void Initialize();
    void Render();

private:
    void CreateDevice();
    void CreateRenderTarget();
    void CreateDepthStencil();
    void CreateViewport();
    void CreateCube();
    void SetShaders();

    HWND m_hwnd;
    int m_width;
    int m_height;

    D3D11_VIEWPORT m_viewport;
    ID3D11Device* m_device;
    ID3D11DeviceContext* m_context;
    IDXGISwapChain* m_swapChain;
    ID3D11RenderTargetView* m_renderTargetView;
    ID3D11DepthStencilView* m_depthStencilView;
    ID3D11Buffer* m_vertexBuffer;
    ID3D11Buffer* m_indexBuffer;
    ID3D11Buffer* m_constantBuffer;
    ID3D11VertexShader* m_vertexShader;
    ID3D11PixelShader* m_pixelShader;
    ID3D11InputLayout* m_inputLayout;

    struct Vertex {
        XMFLOAT3 position;
        XMFLOAT3 color;
    };

    struct ConstantBuffer {
        XMMATRIX worldViewProj;
    };

    XMMATRIX m_world;
    XMMATRIX m_view;
    XMMATRIX m_projection;
};
