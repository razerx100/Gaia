#include <Window.hpp>

int CALLBACK WinMain(
	HINSTANCE,
	HINSTANCE,
	LPSTR,
	int
) {
	try {
		Window wnd(1280, 720, "Window 1");

		MSG msg = {};
		BOOL gResult;
		static int i = 0;
		while ((gResult = GetMessage(&msg, nullptr, 0, 0)) > 0) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (gResult == -1)
			return -1;

		return static_cast<int>(msg.wParam);
	}
	catch (const Xception& e) {
		MessageBox(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
	}
	catch (const std::exception& e) {
		MessageBox(nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...) {
		MessageBox(nullptr, "No details available", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	return -1;
}