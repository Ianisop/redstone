// Lapis-testing.cpp : This file contains the 'main' function. Program execution begins and ends there.

// include the basic windows header files
#include <windows.h>
#include <windowsx.h>
#include <wingdi.h>
#include <algorithm>

#pragma comment(lib, "dwmapi.lib")
#include <dwmapi.h>

// include chrono for time
#include <chrono>
#include <thread>

// include iostream for printing
#include <iostream>
#include "Game.h"
// include DirectX headers
#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <d3dcompiler.h>

// include the Direct3D Library file
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

// include Lapis headers
#include "Lapis/Engine.h"
#include "Lapis/Helpers.h"

// include Utility headers
#include "utils/hsl-to-rgb.hpp"


// the WindowProc function prototype
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// entry point
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {
    
    // define window class
    WNDCLASSEX wc;
    ZeroMemory(&wc, sizeof(WNDCLASSEX));

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    //wc.style = ACS_TRANSPARENT;
    wc.lpfnWndProc = WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = NULL;
    wc.hCursor = LoadCursor(NULL, IDC_NO);
    wc.hbrBackground;
    wc.lpszMenuName;
    wc.lpszClassName = L"LapisWindowClass";
    wc.hIconSm;

    // register the window class
    RegisterClassEx(&wc);

    RECT wr = { 0, 0, 800, 600 };
    HWND hwnd;

    // create the window and use the result as the handle
    hwnd = CreateWindowExW(
        NULL,//( WS_EX_TOPMOST | WS_EX_NOACTIVATE),
        wc.lpszClassName,    // name of the window class
        L"Lapis Dev Window",   // title of the window
        WS_OVERLAPPEDWINDOW,    // window style //WS_POPUP
        300,    // x-position of the window
        300,    // y-position of the window
        wr.right -wr.left,    // width of the window
        wr.bottom -wr.top,    // height of the window
        NULL,    // we have no parent window, NULL
        NULL,    // we aren't using menus, NULL
        hInstance,    // application handle
        NULL);    // used with multiple windows, NULL
    ShowWindow(hwnd, nCmdShow); // make sure window is shown

    //SetLayeredWindowAttributes(hwnd, 0, 1.0f, LWA_ALPHA);
    //SetLayeredWindowAttributes(hwnd, 0, RGB(0, 0, 0), LWA_COLORKEY);
    //MARGINS margins = { -1 }; ;
    //DwmExtendFrameIntoClientArea(hwnd, &margins);

  

    std::cout << "created device and swapchain\n";


#ifdef _DEBUG
    AllocConsole();
    SetConsoleTitleW(L"DEBUG OUTPUT");
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
    printf("yoo!\n");
#endif

    printf("initting lapis\n");
    Lapis::InitLapis(hwnd);


    float FPS_CAP = 60;
    bool LIMIT_FPS = false;
    MSG msg{};
   
    while (true && !GetAsyncKeyState(VK_ESCAPE))
    {
        using namespace Lapis;
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if (msg.message == WM_QUIT)
                break;
        }
        float movementSpeed = 1;

       



        static int checkerboardSize = 25;
        Color col;
        for (int i = 0; i < checkerboardSize; i++) {
            for (int j = 0; j < checkerboardSize; j++) {
                if (((i % 2) + j) % 2 == 1)
                    col  = "ffffff";
                else
                    col = "000000";
                Draw::D3::Plane(Transform(Vec3(i - checkerboardSize / 2, -2, j - checkerboardSize / 2), 0, 1), col);
            }
        }


        Game::Run();




        // Run Lapis Frame
        {
            using namespace Lapis;
            NewFrame();


            RenderFrame();
            FlushFrame();
        }
    }

    std::cout << "Cleaning up";
    Lapis::CleanLapis();

    return (int)msg.wParam;
}

// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    Lapis::WndProcHandler(hwnd, msg, wParam, lParam);

    switch (msg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
        break;

    case WM_MOUSEMOVE:
    
        using namespace Lapis;

        static int xRot{}, yRot{};
        static int xPosOld{ GET_X_LPARAM(lParam) }, yPosOld{ GET_Y_LPARAM(lParam) };

        int xPos = GET_X_LPARAM(lParam);
        int yPos = GET_Y_LPARAM(lParam);

        xRot += (xPos - xPosOld) * deltaTime * 50;
        yRot += (yPos - yPosOld) * deltaTime * 50;
        
        //std::cout << xPos - xPosOld << "\n";
        yRot = std::clamp(yRot, -90, 90);

        

        //std::cout << std::format("{} x, {} y \n", xRot, yRot);
        mainCamera.rot = Vec3(yRot, xRot, 0);

        xPosOld = xPos;
        yPosOld = yPos;
        

        break;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}