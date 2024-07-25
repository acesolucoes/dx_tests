// main.cpp

#include <windows.h>
#include <d3d11.h>
#include <directxmath.h>
#include "CubeRenderer.h"

#pragma comment (lib, "d3d11.lib")

using namespace DirectX;

// Global Declarations
HWND hwnd = NULL;
const int Width = 800;
const int Height = 600;
CubeRenderer* g_CubeRenderer = nullptr;

// Function Prototypes
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void InitWindow(HINSTANCE hInstance, int nCmdShow);
void InitDirect3D();
void CleanUp();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Initialize window
    InitWindow(hInstance, nCmdShow);

    // Initialize Direct3D
    InitDirect3D();

    // Main message loop
    MSG msg = { 0 };
    while (WM_QUIT != msg.message) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } else {
            // Render the cube
            g_CubeRenderer->Render();
        }
    }

    // Clean up
    CleanUp();

    return (int)msg.wParam;
}

void InitWindow(HINSTANCE hInstance, int nCmdShow) {
    // Register the window class
    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = L"DX11WindowClass";

    RegisterClassEx(&wc);

    // Create the window
    RECT rc = { 0, 0, Width, Height };
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    hwnd = CreateWindow(L"DX11WindowClass", L"DirectX 11 Cube", WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top,
                        NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, nCmdShow);
}

void InitDirect3D() {
    g_CubeRenderer = new CubeRenderer(hwnd, Width, Height);
    g_CubeRenderer->Initialize();
}

void CleanUp() {
    delete g_CubeRenderer;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}
