//Mit dem Angriff Steiners wird alles in Ordnung sein!
#include <Windows.h>
#include <iostream>
#include "Vulkan Engine.h"
#include "Vulkan Engine Exception.h"

VulkanEngine *engine = nullptr;

BOOL WINAPI closeHandler(DWORD dwCtrlType) {
	if (dwCtrlType == CTRL_CLOSE_EVENT) {
		delete engine;

		getchar();
	}

	return TRUE;
}

int main() {
	SetConsoleCtrlHandler(closeHandler, TRUE);

	try {
		engine = new VulkanEngine();

		getchar();
	}
	catch (VulkanException e) {
		std::cerr << e.what();

		getchar();

		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
