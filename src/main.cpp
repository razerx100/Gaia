#include <Window.hpp>

int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow
) {
	Window wnd(1280, 720, "Window 1");

	MSG msg = {};
	BOOL gResult;

	while ((gResult = GetMessage(&msg, nullptr, 0, 0)) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (gResult == -1)
		return -1;

	return static_cast<int>(msg.wParam);
}