#include <Window.hpp>
#include <sstream>
#include <resource.hpp>
#include <WindowThrowMacros.hpp>

// WindowClass
Window::WindowClass Window::WindowClass::s_wndClass;

Window::WindowClass::WindowClass() noexcept
	: m_hInst(GetModuleHandle(nullptr)) {

	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = HandleMsgSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetInstance();
	wc.hIcon = LoadIcon(GetInstance(), MAKEINTRESOURCE(IDI_ICON));
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = GetName();
	wc.hIconSm = static_cast<HICON>(LoadImage(GetInstance(), MAKEINTRESOURCE(IDI_ICON), IMAGE_ICON, 16, 16, 0));

	RegisterClassEx(&wc);
}

Window::WindowClass::~WindowClass() {
	UnregisterClass(GetName(), GetInstance());
}

const char* Window::WindowClass::GetName() noexcept {
	return wndClassName;
}

HINSTANCE Window::WindowClass::GetInstance() noexcept {
	return s_wndClass.m_hInst;
}

// Window
Window::Window(int width, int height, const char* name)
	: m_width(width), m_height(height) {
	RECT wr;
	wr.left = 100;
	wr.right = width + wr.left;
	wr.top = 100;
	wr.bottom = height + wr.top;
	if (!AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE))
		throw HWND_LAST_EXCEPT();

	m_hWnd = CreateWindowEx(
		0,
		WindowClass::GetName(), name,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top,
		nullptr, nullptr, WindowClass::GetInstance(), this
	);

	if (!m_hWnd)
		throw HWND_LAST_EXCEPT();

	ShowWindow(m_hWnd, SW_SHOWDEFAULT);

	m_pGfx = std::make_unique<Graphics>(
		m_hWnd, static_cast<std::uint32_t>(width), static_cast<std::uint32_t>(height)
		);
}

Window::~Window() {
	DestroyWindow(m_hWnd);
}

LRESULT CALLBACK Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept {
	if (msg == WM_NCCREATE) {
		const CREATESTRUCT* const pCreate = reinterpret_cast<CREATESTRUCTA*>(lParam);
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgWrap));

		return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
	}

	return DefWindowProcA(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK Window::HandleMsgWrap(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept {
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}

LRESULT Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept {
	switch (msg) {
	case WM_CLOSE: {
		PostQuitMessage(0);
		return 0;
	}
	// Clear keystate when window loses focus to prevent input getting stuck
	case WM_KILLFOCUS: {
		m_kb.ClearState();
		break;
	}
	/************* KEYBOARD MESSAGES *************/
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN: {
		if (!(lParam & 0x40000000) || m_kb.IsAutoRepeatEnabled()) // filters autoRepeat
			m_kb.OnKeyPressed(static_cast<unsigned char>(wParam));
		break;
	}
	case WM_KEYUP:
	case WM_SYSKEYUP: {
		m_kb.OnKeyReleased(static_cast<unsigned char>(wParam));
		break;
	}
	case WM_CHAR: {
		m_kb.OnChar(static_cast<char>(wParam));
		break;
	}
	/************* END KEYBOARD MESSAGES *************/
	/************* MOUSE MESSAGES *************/
	case WM_MOUSEMOVE: {
		const POINTS pt = MAKEPOINTS(lParam);
		// In client region
		if (pt.x >= 0 && pt.x < m_width && pt.y >= 0 && pt.y < m_height) {
			m_mouse.OnMouseMove(pt.x, pt.y);

			if (!m_mouse.IsInWindow()) {
				SetCapture(m_hWnd);
				m_mouse.OnMouseEnter();
			}
		}
		// Not in client region, log move if button down
		else {
			if(wParam & (MK_LBUTTON | MK_RBUTTON | MK_MBUTTON))
				m_mouse.OnMouseMove(pt.x, pt.y);
			else {
				ReleaseCapture();
				m_mouse.OnMouseLeave();
			}
		}
		break;
	}
	case WM_LBUTTONDOWN: {
		m_mouse.OnLeftPress();
		break;
	}
	case WM_LBUTTONUP: {
		m_mouse.OnLeftRelease();
		break;
	}
	case WM_MBUTTONDOWN: {
		m_mouse.OnMiddlePress();
		break;
	}
	case WM_MBUTTONUP: {
		m_mouse.OnMiddleRelease();
		break;
	}
	case WM_RBUTTONDOWN: {
		m_mouse.OnRightPress();
		break;
	}
	case WM_RBUTTONUP: {
		m_mouse.OnRightRelease();
		break;
	}
	case WM_MOUSEWHEEL: {
		int deltaWparam = GET_WHEEL_DELTA_WPARAM(wParam);

		m_mouse.OnWheelDelta(deltaWparam);
		break;
	}
	/************* END MOUSE MESSAGES *************/
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

void Window::SetTitle(const std::string& title) {
	if (!SetWindowText(m_hWnd, title.c_str()))
		throw HWND_LAST_EXCEPT();
}

std::optional<int> Window::ProcessMessages() {
	MSG msg = {};

	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
		if (msg.message == WM_QUIT)
			return static_cast<int>(msg.wParam);

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return {};
}

Graphics& Window::GetGfx() const {
	if (!m_pGfx)
		throw HWND_NOGFX_EXCEPT();

	return *m_pGfx;
}

