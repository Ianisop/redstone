// Lapis-testing.cpp : This file contains the 'main' function. Program execution begins and ends there.

// include the basic windows header files
#include <windows.h>
#include <windowsx.h>
#include <wingdi.h>
#include <algorithm>
#include <hidusage.h>
#include <numeric> // For std::accumulate
#include <deque>
#pragma comment(lib, "dwmapi.lib")
#include <dwmapi.h>
#include <iomanip> // Include this for std::setprecision and std::fixed

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
#include "src/Lapis/Engine.h"
#include "src/Lapis/Helpers.h"

// include Utility headers
#include "src/utils/hsl-to-rgb.hpp"

#include "src/imgui/imgui.h"
#include "src/imgui/backends/imgui_impl_win32.h"
#include "src/imgui/backends/imgui_impl_dx11.h"

#define MAX_FLOAT = 340282346638528859811704183484516925440.0000000000000000;
bool SHOW_CURSOR = false;

// the WindowProc function prototype
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// entry point
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {

    // define window class
    WNDCLASSEX wc;
    ZeroMemory(&wc, sizeof(WNDCLASSEX));

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = NULL;
    wc.hCursor = LoadCursor(NULL, IDC_NO);
    wc.hbrBackground = NULL;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = L"LapisWindowClass";
    wc.hIconSm = NULL;

    // register the window class
    RegisterClassEx(&wc);

    RECT wr = { 0, 0, 800, 600 };
    HWND hwnd;

    // create the window and use the result as the handle
    hwnd = CreateWindowExW(
        NULL, //( WS_EX_TOPMOST | WS_EX_NOACTIVATE),
        wc.lpszClassName,    // name of the window class
        L"Lapis Dev Window",   // title of the window
        WS_OVERLAPPEDWINDOW,    // window style //WS_POPUP
        300,    // x-position of the window
        300,    // y-position of the window
        wr.right - wr.left,    // width of the window
        wr.bottom - wr.top,    // height of the window
        NULL,    // we have no parent window, NULL
        NULL,    // we aren't using menus, NULL
        hInstance,    // application handle
        NULL);    // used with multiple windows, NULL
    ShowWindow(hwnd, nCmdShow); // make sure window is shown

    //SetLayeredWindowAttributes(hwnd, 0, 1.0f, LWA_ALPHA);
    //SetLayeredWindowAttributes(hwnd, 0, RGB(0, 0, 0), LWA_COLORKEY);
    //MARGINS margins = { -1, -1, -1, -1 }; ;
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

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init(hwnd);

    ID3D11Device* device;
    ID3D11DeviceContext* deviceContext;
    Lapis::GetDeviceAndCtx(&device, &deviceContext);
    ImGui_ImplDX11_Init(device, deviceContext);

    RAWINPUTDEVICE rid[1];
    rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
    rid[0].usUsage = HID_USAGE_GENERIC_MOUSE;
    rid[0].dwFlags = RIDEV_INPUTSINK;        // do not generate legacy messages such as WM_KEYDOWN
    rid[0].hwndTarget = hwnd;
    RegisterRawInputDevices(rid, 1, sizeof(rid[0]));


    constexpr float FPS_CAP = std::numeric_limits<float>::infinity(); // 60 by default

    constexpr float FRAME_TIME_MS = 1000.0f / FPS_CAP;


    auto lastTime = std::chrono::high_resolution_clock::now();

    // To keep track of recent FPS values
    std::deque<float> fpsHistory;
    constexpr int FPS_HISTORY_SIZE = 60; // Number of frames to average for stable FPS
    MSG msg{};
    while (true && !GetAsyncKeyState(VK_ESCAPE))
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if (msg.message == WM_QUIT)
                break;
        }

        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float, std::milli> deltaTime = currentTime - lastTime;

        if (deltaTime.count() >= FRAME_TIME_MS)
        {
            lastTime = currentTime;

            // Calculate the current FPS and store it in the history
            float currentFps = 1000.0f / deltaTime.count();
            fpsHistory.push_back(currentFps);

            // Limit the history size to FPS_HISTORY_SIZE
            if (fpsHistory.size() > FPS_HISTORY_SIZE)
            {
                fpsHistory.pop_front();
            }

            // Calculate the average FPS from the history
            float averageFps = std::accumulate(fpsHistory.begin(), fpsHistory.end(), 0.0f) / fpsHistory.size();
        }

        // Calculate the average FPS from the history
        float averageFps = std::accumulate(fpsHistory.begin(), fpsHistory.end(), 0.0f) / fpsHistory.size();

        // Run Lapis Frame
        {
            using namespace Lapis;
            NewFrame();

            // Create a stringstream to format the average FPS value
            std::stringstream fpsStream;
            fpsStream << std::fixed << std::setprecision(0) << averageFps;
            std::string fpsStringvalue = fpsStream.str();

            const char* c = fpsStringvalue.c_str(); // c = char pointer
            Draw::D2::String(c, { 5,5 }, "ff0000", 3);


            if (GetAsyncKeyState('L') && 0x01)
            {
                SHOW_CURSOR = !SHOW_CURSOR;
                std::cout << SHOW_CURSOR << "\n";
            }

            RenderFrame();
            FlushFrame();
        }
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Player");

        ImGui::End();
        ImGui::Render();
        Game::Run();

        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        Lapis::PresentFrame();
    }

    std::cout << "Cleaning up";

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    Lapis::CleanLapis();

    return (int)msg.wParam;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    Lapis::WndProcHandler(hwnd, msg, wParam, lParam);

    switch (msg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
        break;

    case WM_LBUTTONDOWN:
        SHOW_CURSOR = !SHOW_CURSOR;
        std::cout << SHOW_CURSOR << "\n";
        break;

    case WM_INPUT:
        using namespace Lapis;
        UINT dwSize = sizeof(RAWINPUT);
        static BYTE lpb[sizeof(RAWINPUT)];
        int xPosRelative{};
        int yPosRelative{};
        static int xRot{}, yRot{};

        GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));

        RAWINPUT* raw = (RAWINPUT*)lpb;

        if (raw->header.dwType == RIM_TYPEMOUSE)
        {
            xPosRelative = raw->data.mouse.lLastX;
            yPosRelative = raw->data.mouse.lLastY;
        }

        xRot -= xPosRelative * 1;
        yRot -= yPosRelative * 1;
        POINT p = { clientRect.width / 2, clientRect.height / 2 };

        ClientToScreen(hwnd, &p);

        yRot = std::clamp(yRot, -90, 90);

        //std::cout << std::format("{} x, {} y \n", xRot, yRot);
        mainCamera.rot = Vec3(yRot, xRot, 0);

        if(!SHOW_CURSOR)SetCursorPos(p.x, p.y);
        ShowCursor(SHOW_CURSOR);
        Game::Input();
        break;
  
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }


}