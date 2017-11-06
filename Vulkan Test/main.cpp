//Mit dem Angriff Steiners wird alles in Ordnung sein!
#include <Windows.h>
#include <iostream>
#include "Vulkan Engine.h"

VulkanEngine *engine = nullptr;
const char g_szClassName[] = "myWindowClass";
HWND windowHandle = NULL;
bool vulkanInitied = false;

BOOL WINAPI closeHandler(DWORD dwCtrlType) {
	if (dwCtrlType == CTRL_CLOSE_EVENT) {
		delete engine;
	}

	return TRUE;
}

bool initVulkan(HINSTANCE hInstance, HWND windowHandle) {
	try {
		engine = new VulkanEngine(hInstance, windowHandle);
	}
	catch (VulkanException e) {
		std::cerr << e.what();

		getchar();

		return false;
	}
	
	vulkanInitied = true;

	return true;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_SHOWWINDOW:
		if (!vulkanInitied && !initVulkan(GetModuleHandle(NULL), windowHandle))
			return -1;
		break;
	case WM_CLOSE:
		delete engine;
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

void initWindow(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wc;
	MSG Msg;

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
		WS_EX_CLIENTEDGE,
		g_szClassName,
		"Vulkan Test",
		WS_OVERLAPPEDWINDOW ^ (WS_THICKFRAME | WS_MAXIMIZEBOX),
		CW_USEDEFAULT, CW_USEDEFAULT, 1024, 768,
		NULL, NULL, hInstance, NULL);

	if (windowHandle == NULL)
		throw std::exception("Window creation failed.");

	ShowWindow(windowHandle, nCmdShow);
	UpdateWindow(windowHandle);

	while (GetMessage(&Msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}

	return;
}


int main() {
	SetConsoleCtrlHandler(closeHandler, TRUE);

	initWindow(GetModuleHandle(NULL), NULL, "", 1);

	return EXIT_SUCCESS;
}