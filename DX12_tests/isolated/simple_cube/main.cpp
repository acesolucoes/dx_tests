#include <windows.h>
#include <wrl.h>
#include <Shlwapi.h>
#include <io.h>
#include <fcntl.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

// D3D12 extension library.
#include <d3dx12.h>

#include <comdef.h> // For _com_error class (used to decode HR result codes).

#include <string>
#include <chrono>
#include <iostream>



using namespace Microsoft::WRL;
using namespace DirectX;

// Window settings
const UINT Width = 800;
const UINT Height = 600;

// Pipeline objects
ComPtr<ID3D12Device> device;
ComPtr<IDXGISwapChain3> swapChain;
ComPtr<ID3D12CommandQueue> commandQueue;
ComPtr<ID3D12DescriptorHeap> rtvHeap;
ComPtr<ID3D12Resource> renderTargets[2];
ComPtr<ID3D12CommandAllocator> commandAllocator;
ComPtr<ID3D12RootSignature> rootSignature;
ComPtr<ID3D12PipelineState> pipelineState;
ComPtr<ID3D12GraphicsCommandList> commandList;
ComPtr<ID3D12Fence> fence;
HANDLE fenceEvent;
UINT64 fenceValue = 0;

UINT rtvDescriptorSize;
UINT frameIndex;

// Cube resources
struct Vertex {
    XMFLOAT3 position;
    XMFLOAT4 color;
};
ComPtr<ID3D12Resource> vertexBuffer;
D3D12_VERTEX_BUFFER_VIEW vertexBufferView;

ComPtr<ID3D12Resource> constantBuffer;
UINT8* pCbvDataBegin;

struct ConstantBuffer {
    XMFLOAT4X4 mvp;
};
ConstantBuffer cbData;

// Timing
auto startTime = std::chrono::high_resolution_clock::now();

// Forward declarations
void PopulateCommandList();
void WaitForPreviousFrame();
void Update();

// Window procedure
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}


inline void ThrowIfFailed(HRESULT hr)
{
    if (FAILED(hr))
    {
        _com_error err(hr);
        OutputDebugString(err.ErrorMessage());

        throw std::exception("error");
    }
}

static void CreateConsole()
{
    // Allocate a console.
    if (AllocConsole())
    {
        constexpr int MAX_CONSOLE_LINES = 500;

        HANDLE lStdHandle = GetStdHandle(STD_OUTPUT_HANDLE);

        // Increase screen buffer to allow more lines of text than the default.
        CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
        GetConsoleScreenBufferInfo(lStdHandle, &consoleInfo);
        consoleInfo.dwSize.Y = MAX_CONSOLE_LINES;
        SetConsoleScreenBufferSize(lStdHandle, consoleInfo.dwSize);
        SetConsoleCursorPosition(lStdHandle, { 0, 0 });

        // Redirect unbuffered STDOUT to the console.
        int   hConHandle = _open_osfhandle((intptr_t)lStdHandle, _O_TEXT);
        FILE* fp = _fdopen(hConHandle, "w");
        freopen_s(&fp, "CONOUT$", "w", stdout);
        setvbuf(stdout, nullptr, _IONBF, 0);

        // Redirect unbuffered STDIN to the console.
        lStdHandle = GetStdHandle(STD_INPUT_HANDLE);
        hConHandle = _open_osfhandle((intptr_t)lStdHandle, _O_TEXT);
        fp = _fdopen(hConHandle, "r");
        freopen_s(&fp, "CONIN$", "r", stdin);
        setvbuf(stdin, nullptr, _IONBF, 0);

        // Redirect unbuffered STDERR to the console.
        lStdHandle = GetStdHandle(STD_ERROR_HANDLE);
        hConHandle = _open_osfhandle((intptr_t)lStdHandle, _O_TEXT);
        fp = _fdopen(hConHandle, "w");
        freopen_s(&fp, "CONOUT$", "w", stderr);
        setvbuf(stderr, nullptr, _IONBF, 0);

        // Clear the error state for each of the C++ standard stream objects. We
        // need to do this, as attempts to access the standard streams before
        // they refer to a valid target will cause the iostream objects to enter
        // an error state. In versions of Visual Studio after 2005, this seems
        // to always occur during startup regardless of whether anything has
        // been read from or written to the console or not.
        std::wcout.clear();
        std::cout.clear();
        std::wcerr.clear();
        std::cerr.clear();
        std::wcin.clear();
        std::cin.clear();
    }
}

// Main
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    // enable debug layer
    ComPtr<ID3D12Debug> debugInterface;
    ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface)));
    debugInterface->EnableDebugLayer();

    CreateConsole();

    // Register window class
    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"DX12WindowClass";
    RegisterClass(&wc);

    // Create window
    HWND hwnd = CreateWindowEx(0, wc.lpszClassName, L"DX12 Rotating Cube",
                               WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                               Width, Height, nullptr, nullptr, hInstance, nullptr);
    ShowWindow(hwnd, nCmdShow);

    // --- DirectX 12 initialization ---
    ComPtr<IDXGIFactory4> factory;
    CreateDXGIFactory1(IID_PPV_ARGS(&factory));

    D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));

    D3D12_COMMAND_QUEUE_DESC cqDesc = {};
    device->CreateCommandQueue(&cqDesc, IID_PPV_ARGS(&commandQueue));

    DXGI_SWAP_CHAIN_DESC1 scDesc = {};
    scDesc.BufferCount = 2;
    scDesc.Width = Width;
    scDesc.Height = Height;
    scDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    scDesc.SampleDesc.Count = 1;

    ComPtr<IDXGISwapChain1> tempSwapChain;
    factory->CreateSwapChainForHwnd(commandQueue.Get(), hwnd, &scDesc, nullptr, nullptr, &tempSwapChain);
    tempSwapChain.As(&swapChain);
    frameIndex = swapChain->GetCurrentBackBufferIndex();

    // RTV heap
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.NumDescriptors = 2;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap));
    rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    // Create frame resources
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart());
    for (UINT n = 0; n < 2; n++) {
        swapChain->GetBuffer(n, IID_PPV_ARGS(&renderTargets[n]));
        device->CreateRenderTargetView(renderTargets[n].Get(), nullptr, rtvHandle);
        rtvHandle.Offset(1, rtvDescriptorSize);
    }
    device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));

    // Root signature
    D3D12_ROOT_SIGNATURE_DESC rootSigDesc = {};
    rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    ComPtr<ID3DBlob> sigBlob, errBlob;
    D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &sigBlob, &errBlob);
    device->CreateRootSignature(0, sigBlob->GetBufferPointer(), sigBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));

    // Compile shaders
    ComPtr<ID3DBlob> vs, ps;
    D3DCompileFromFile(L"cube.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", 0, 0, &vs, nullptr);
    D3DCompileFromFile(L"cube.hlsl", nullptr, nullptr, "PSMain", "ps_5_0", 0, 0, &ps, nullptr);

    // Input layout
    D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,   D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

    // Pipeline state
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.InputLayout = { inputLayout, _countof(inputLayout) };
    psoDesc.pRootSignature = rootSignature.Get();
    psoDesc.VS = { vs->GetBufferPointer(), vs->GetBufferSize() };
    psoDesc.PS = { ps->GetBufferPointer(), ps->GetBufferSize() };
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psoDesc.DepthStencilState.DepthEnable = FALSE;
    psoDesc.DepthStencilState.StencilEnable = FALSE;
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    psoDesc.SampleDesc.Count = 1;

    device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState));

    // Command list
    device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator.Get(),
                              pipelineState.Get(), IID_PPV_ARGS(&commandList));
    commandList->Close();

    // Vertex buffer (cube)
    Vertex cubeVertices[] = {
        // Front face
        { {-1.0f,-1.0f,-1.0f}, {1,0,0,1} },
        { {-1.0f, 1.0f,-1.0f}, {0,1,0,1} },
        { { 1.0f, 1.0f,-1.0f}, {0,0,1,1} },
        { { 1.0f,-1.0f,-1.0f}, {1,1,0,1} },
        // Back face
        { {-1.0f,-1.0f,1.0f}, {1,0,1,1} },
        { {-1.0f, 1.0f,1.0f}, {0,1,1,1} },
        { { 1.0f, 1.0f,1.0f}, {1,1,1,1} },
        { { 1.0f,-1.0f,1.0f}, {0.5f,0.2f,0.7f,1} }
    };
    const UINT vbSize = sizeof(cubeVertices);

    CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
    CD3DX12_RESOURCE_DESC bufDesc = CD3DX12_RESOURCE_DESC::Buffer(vbSize);
    device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE,
                                    &bufDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
                                    nullptr, IID_PPV_ARGS(&vertexBuffer));

    UINT8* vbDataBegin;
    CD3DX12_RANGE readRange(0, 0);
    vertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&vbDataBegin));
    memcpy(vbDataBegin, cubeVertices, vbSize);
    vertexBuffer->Unmap(0, nullptr);

    vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
    vertexBufferView.StrideInBytes = sizeof(Vertex);
    vertexBufferView.SizeInBytes = vbSize;

    auto buff = CD3DX12_RESOURCE_DESC::Buffer(1024 * 64);

    // Constant buffer
    device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE,
        &buff,
        D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&constantBuffer));

    constantBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pCbvDataBegin));

    // Fence
    device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
    fenceValue = 1;
    fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

    std::cout << "Test print\n";

    // --- Main loop ---
    MSG msg = {};
    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } else {
            Update();
            PopulateCommandList();
            ID3D12CommandList* lists[] = { commandList.Get() };
            commandQueue->ExecuteCommandLists(_countof(lists), lists);
            swapChain->Present(1, 0);
            WaitForPreviousFrame();
        }
    }

    WaitForPreviousFrame();
    CloseHandle(fenceEvent);
    return 0;
}

// Update rotation
void Update() {
    auto now = std::chrono::high_resolution_clock::now();
    float t = std::chrono::duration<float>(now - startTime).count();

    XMMATRIX model = XMMatrixRotationY(t);
    XMMATRIX view = XMMatrixLookAtLH({0,0,-5}, {0,0,0}, {0,1,0});
    XMMATRIX proj = XMMatrixPerspectiveFovLH(XM_PIDIV4, float(Width)/Height, 0.1f, 100.0f);
    XMMATRIX mvp = model * view * proj;
    XMStoreFloat4x4(&cbData.mvp, XMMatrixTranspose(mvp));

    memcpy(pCbvDataBegin, &cbData, sizeof(cbData));
}

// Record commands
void PopulateCommandList() {
    commandAllocator->Reset();
    commandList->Reset(commandAllocator.Get(), pipelineState.Get());

    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        renderTargets[frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT,
        D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandList->ResourceBarrier(1, &barrier);

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart(),
                                            frameIndex, rtvDescriptorSize);

    const float clearColor[] = {0.1f, 0.2f, 0.4f, 1.0f};
    commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
    commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

    commandList->SetGraphicsRootSignature(rootSignature.Get());
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
    commandList->DrawInstanced(8, 1, 0, 0);

    barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        renderTargets[frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_PRESENT);
    commandList->ResourceBarrier(1, &barrier);

    commandList->Close();
}

// Sync
void WaitForPreviousFrame() {
    const UINT64 fv = fenceValue;
    commandQueue->Signal(fence.Get(), fv);
    fenceValue++;
    if (fence->GetCompletedValue() < fv) {
        fence->SetEventOnCompletion(fv, fenceEvent);
        WaitForSingleObject(fenceEvent, INFINITE);
    }
    frameIndex = swapChain->GetCurrentBackBufferIndex();
}
