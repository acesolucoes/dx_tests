#include "pch.h"
//#include "platform/WIN32/WinEntry.h"

#define MAX_NAME_STRING 256
#define HInstance() GetModuleHandle(NULL)

WCHAR WindowClass[MAX_NAME_STRING];
WCHAR WindowTitle[MAX_NAME_STRING];

INT WindowWidth;
INT WindowHeight;

LRESULT CALLBACK WindowProcess(HWND hWnd, UINT message, WPARAM wparam, LPARAM lparam)
{
    switch(message) {
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
    }

    return DefWindowProc(hWnd, message, wparam, lparam);
}

VOID InitializeVariables();
VOID CreateWindowClass();

/*
int CALLBACK WinMain(HINSTANCE, HINSTANCE, LPSTR, INT)
{

    InitializeVariables();

    CreateWindowClass();

    VOID MessageLoop();

    

    HWND hWnd = CreateWindow(WindowClass, WindowTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, WindowWidth, WindowHeight, nullptr, nullptr, HInstance(), nullptr);

    if( !hWnd ) {
        MessageBox(0, L"failed to Create Window!.", 0, 0);
        return 0;
    }

    ShowWindow(hWnd, SW_SHOW);

    MessageLoop();
    

    return 0;
}
*/

VOID InitializeVariables() {
    wcscpy_s(WindowClass, TEXT("TutorialOneClass"));
    wcscpy_s(WindowTitle, TEXT("Our First Window"));

    WindowWidth = 1366;
    WindowHeight = 768;
}

VOID CreateWindowClass() {
    
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;

    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH) GetStockObject(NULL_BRUSH);

    wcex.hIcon = LoadIcon(0, IDI_APPLICATION);
    wcex.hIconSm = LoadIcon(0, IDI_APPLICATION);

    wcex.lpszClassName = WindowClass;

    wcex.lpszMenuName = nullptr;

    wcex.hInstance = HInstance();

    wcex.lpfnWndProc = WindowProcess;

    RegisterClassEx(&wcex);
}

VOID MessageLoop() {
    MSG msg = { 0 };
    while( msg.message != WM_QUIT )
    {
        // If there are WIndow messages then process them.
        if( PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}