//Mit dem Angriff Steiners wird alles in Ordnung sein!
#include <Windows.h>
#include <thread>
#include <iostream>
#include "Vulkan Engine.h"
//#define FULLSCREEN

VulkanEngine *engine = NULL;
const char g_szClassName[] = "VulkanEngine Test Window Class";
HWND windowHandle = NULL;
bool vulkanInited = false;
VulkanEngine **pUnstableInstance = new VulkanEngine *;
bool quitMessageReceived = false;

void deleteEngineOrUnstableEngine() {
    if (*pUnstableInstance != NULL)
        delete *pUnstableInstance;
    else
        delete engine;
}

BOOL WINAPI closeHandler(DWORD dwCtrlType) {
    if (!engine->terminating) {
        if (dwCtrlType == CTRL_CLOSE_EVENT) {
            engine->terminating = true;

            deleteEngineOrUnstableEngine();

            quitMessageReceived = true;
        }
    }

    return TRUE;
}

bool initVulkanReal(HINSTANCE hInstance, HWND windowHandle) {
    try {
        engine = new VulkanEngine(hInstance, windowHandle, pUnstableInstance);

        if (engine != NULL)
            *pUnstableInstance = NULL;
    }
    catch (VulkanException e) {
        std::cerr << e.what();

        getchar();

        delete *pUnstableInstance;

        exit(EXIT_FAILURE);
    }
}

bool initVulkan(HINSTANCE hInstance, HWND windowHandle) {
    initVulkanReal(hInstance, windowHandle);

    vulkanInited = true;

    return true;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_SHOWWINDOW:
#ifndef NDEBUG
            LoadLibrary("DevIL_debug.dll");
#else
            LoadLibrary("DevIL.dll");
#endif // DEBUG
            ilInit();

            std::cout << "DevIL library inited." << std::endl;

            if (!vulkanInited && !initVulkan(GetModuleHandle(NULL), windowHandle))
                return -1;
            else
                return DefWindowProc(hwnd, msg, wParam, lParam);
        case WM_CLOSE:
            if (!engine->terminating) {
                engine->terminating = true;
                deleteEngineOrUnstableEngine();
                DestroyWindow(hwnd);
            }
            break;
        case WM_DESTROY:
            engine->terminating = true;
            PostQuitMessage(0);
            quitMessageReceived = true;
            break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

//extern uint8_t data[]   asm("_binary_Resources_0_png_start");
//extern uint8_t size[]   asm("_binary_Resources_0_png_size");
//extern uint8_t end[]    asm("_binary_Resources_0_png_end");

//void extractResources() {
//    size_t realSize = (size_t)((void *)size);
//
//    FILE *file;
//
//    file = fopen("0.png", "wb");
//    fwrite(data, 1, realSize, file); //64 Bit only
//    fclose(file);
//}

void initWindow(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                LPSTR lpCmdLine, int nCmdShow) {

//    extractResources();

    WNDCLASSEX wc;
    MSG msg;
#ifdef FULLSCREEN
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = g_szClassName;
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wc))
        throw std::exception("Window registration failed.");

    windowHandle = CreateWindowEx(
        WS_EX_TOPMOST,
        g_szClassName,
        "VulkanEngine Test",
        WS_POPUP,
        0, 0, 1920, 1080,
        NULL, NULL, hInstance, NULL);

    ShowWindow(windowHandle, nCmdShow);
    UpdateWindow(windowHandle);

    DEVMODE screen;
    memset(&screen, 0, sizeof(screen));
    screen.dmSize = sizeof(screen);
    screen.dmPelsWidth = 1920;
    screen.dmPelsHeight = 1080;
    screen.dmBitsPerPel = 32;
    screen.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
    ChangeDisplaySettings(&screen, CDS_FULLSCREEN);

    ShowCursor(false);
#else
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = g_szClassName;
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wc))
        throw std::exception();

    windowHandle = CreateWindowEx(
            WS_EX_CLIENTEDGE,
            g_szClassName,
            "Vulkan Test",
            WS_OVERLAPPEDWINDOW ^ (WS_THICKFRAME | WS_MAXIMIZEBOX),
            CW_USEDEFAULT, CW_USEDEFAULT, 1100, 1000,
            NULL, NULL, hInstance, NULL);

    ShowWindow(windowHandle, nCmdShow);
    UpdateWindow(windowHandle);
#endif

    if (windowHandle == NULL)
        throw std::exception();


    while (!quitMessageReceived) {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if (msg.message == WM_QUIT) {
                quitMessageReceived = true;
                engine->terminating = true;
                break;
            }
        }
        if (!engine->terminating)
            engine->draw();
    }

    return;
}


int main() {
    SetConsoleCtrlHandler(closeHandler, TRUE);

    //Consequently, The first WM_SHOWWINDOW message starts the VulkanEngine initialization.
//    initWindow(GetModuleHandle(NULL), NULL, "", 1);
    initWindow(GetModuleHandle(NULL), NULL, (LPSTR) "", 1);

    return EXIT_SUCCESS;
}