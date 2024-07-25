#include "CubeRenderer.h"
#include "ShaderLoader.h"

CubeRenderer::CubeRenderer(HWND hwnd, int width, int height)
    : m_hwnd(hwnd), m_width(width), m_height(height),
      m_device(nullptr), m_context(nullptr), m_swapChain(nullptr),
      m_renderTargetView(nullptr), m_depthStencilView(nullptr),
      m_vertexBuffer(nullptr), m_indexBuffer(nullptr),
      m_constantBuffer(nullptr), m_vertexShader(nullptr),
      m_pixelShader(nullptr), m_inputLayout(nullptr) {
}

CubeRenderer::~CubeRenderer() {
    if (m_context) m_context->ClearState();
    if (m_swapChain) m_swapChain->Release();
    if (m_renderTargetView) m_renderTargetView->Release();
    if (m_depthStencilView) m_depthStencilView->Release();
    if (m_vertexBuffer) m_vertexBuffer->Release();
    if (m_indexBuffer) m_indexBuffer->Release();
    if (m_constantBuffer) m_constantBuffer->Release();
    if (m_vertexShader) m_vertexShader->Release();
    if (m_pixelShader) m_pixelShader->Release();
    if (m_inputLayout) m_inputLayout->Release();
    if (m_context) m_context->Release();
    if (m_device) m_device->Release();
}

void CubeRenderer::Initialize() {
    CreateDevice();
    CreateRenderTarget();
    CreateDepthStencil();
    CreateViewport();
    CreateCube();
    SetShaders();
}

void CubeRenderer::CreateDevice() {
    DXGI_SWAP_CHAIN_DESC scd = {};
    scd.BufferCount = 1;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow = m_hwnd;
    scd.SampleDesc.Count = 1;
    scd.Windowed = TRUE;

    D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, NULL, 0,
                                  D3D11_SDK_VERSION, &scd, &m_swapChain, &m_device, NULL, &m_context);
}

void CubeRenderer::CreateRenderTarget() {
    ID3D11Texture2D* backBuffer;
    m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
    m_device->CreateRenderTargetView(backBuffer, NULL, &m_renderTargetView);
    backBuffer->Release();
}

void CubeRenderer::CreateDepthStencil() {
    D3D11_TEXTURE2D_DESC descDepth = {};
    descDepth.Width = m_width;
    descDepth.Height = m_height;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    ID3D11Texture2D* depthStencil;
    m_device->CreateTexture2D(&descDepth, NULL, &depthStencil);

    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    m_device->CreateDepthStencilView(depthStencil, &descDSV, &m_depthStencilView);
    depthStencil->Release();
}

void CubeRenderer::CreateViewport() {
    m_viewport.Width = (FLOAT)m_width;
    m_viewport.Height = (FLOAT)m_height;
    m_viewport.MinDepth = 0.0f;
    m_viewport.MaxDepth = 1.0f;
    m_viewport.TopLeftX = 0.0f;
    m_viewport.TopLeftY = 0.0f;

    m_context->RSSetViewports(1, &m_viewport);
}

void CubeRenderer::CreateCube() {
    Vertex vertices[] = {
        { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) },
        { XMFLOAT3(-1.0f,  1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
        { XMFLOAT3( 1.0f,  1.0f, -1.0f), XMFLOAT3(1.0f, 1.0f, 0.0f) },
        { XMFLOAT3( 1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
        { XMFLOAT3(-1.0f, -1.0f,  1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
        { XMFLOAT3(-1.0f,  1.0f,  1.0f), XMFLOAT3(0.0f, 1.0f, 1.0f) },
        { XMFLOAT3( 1.0f,  1.0f,  1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f) },
        { XMFLOAT3( 1.0f, -1.0f,  1.0f), XMFLOAT3(1.0f, 0.0f, 1.0f) }
    };

    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(vertices);
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData = {};
    InitData.pSysMem = vertices;

    m_device->CreateBuffer(&bd, &InitData, &m_vertexBuffer);

    UINT indices[] = {
        0, 1, 2, 0, 2, 3,
        4, 5, 6, 4, 6, 7,
        4, 5, 1, 4, 1, 0,
        7, 6, 2, 7, 2, 3,
        5, 1, 2, 5, 2, 6,
        4, 0, 3, 4, 3, 7
    };

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(indices);
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;

    InitData.pSysMem = indices;

    m_device->CreateBuffer(&bd, &InitData, &m_indexBuffer);

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(ConstantBuffer);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;

    m_device->CreateBuffer(&bd, NULL, &m_constantBuffer);

    m_world = XMMatrixIdentity();
    m_view = XMMatrixLookAtLH(XMVectorSet(0.0f, 2.0f, -5.0f, 1.0f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
    m_projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, m_width / (FLOAT)m_height, 0.01f, 100.0f);
}

void CubeRenderer::SetShaders() {
    // Load and compile the vertex and pixel shaders
    ID3DBlob* vsBlob = nullptr;
    ID3DBlob* psBlob = nullptr;

    ShaderLoader::CompileShader(L"VertexShader.hlsl", "main", "vs_4_0", &vsBlob);
    ShaderLoader::CompileShader(L"PixelShader.hlsl", "main", "ps_4_0", &psBlob);

    m_device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), NULL, &m_vertexShader);
    m_device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), NULL, &m_pixelShader);

    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    m_device->CreateInputLayout(layout, ARRAYSIZE(layout), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &m_inputLayout);

    vsBlob->Release();
    psBlob->Release();
}

void CubeRenderer::Render() {
    // Clear the back buffer and depth buffer
    float clearColor[4] = { 0.0f, 0.2f, 0.4f, 1.0f };
    m_context->ClearRenderTargetView(m_renderTargetView, clearColor);
    m_context->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

    // Update the world-view-projection matrix
    ConstantBuffer cb;
    cb.worldViewProj = XMMatrixTranspose(m_world * m_view * m_projection);
    m_context->UpdateSubresource(m_constantBuffer, 0, NULL, &cb, 0, 0);

    // Set the input layout
    m_context->IASetInputLayout(m_inputLayout);

    // Set vertex buffer
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    m_context->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

    // Set index buffer
    m_context->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Set primitive topology
    m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Set the vertex and pixel shaders
    m_context->VSSetShader(m_vertexShader, NULL, 0);
    m_context->VSSetConstantBuffers(0, 1, &m_constantBuffer);
    m_context->PSSetShader(m_pixelShader, NULL, 0);

    // Draw the cube
    m_context->DrawIndexed(36, 0, 0);

    // Present the buffer
    m_swapChain->Present(1, 0);
}
