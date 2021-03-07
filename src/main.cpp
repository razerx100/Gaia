#include <Window.hpp>

int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow
) {
	try {
		Window wnd(1280, 720, "Window 1");

		MSG msg = {};
		BOOL gResult;

		while ((gResult = GetMessage(&msg, nullptr, 0, 0)) > 0) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (wnd.m_kb.IsKeyPressed(VK_MENU))
				MessageBox(nullptr, "Something happened.", "The Alt key was pressed", MB_OK | MB_ICONEXCLAMATION);
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