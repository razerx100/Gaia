#include <Window.hpp>
#include <sstream>
#include <resource.hpp>
#include <WindowThrowMacros.hpp>
#include <ImGuiImpl.hpp>

#ifdef _IMGUI
// Forward Declaration of ImGui wndproc
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

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
	: m_width(width), m_height(height), m_fullScreenMode(false),
	m_windowStyle(WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU),
	m_cursorEnabled(true) {
	RECT wr;
	wr.left = 0;
	wr.right = width;
	wr.top = 0;
	wr.bottom = height;
	if (!AdjustWindowRect(&wr, m_windowStyle, FALSE))
		throw HWND_LAST_EXCEPT();

	m_hWnd = CreateWindowEx(
		0,
		WindowClass::GetName(), name,
		m_windowStyle,
		CW_USEDEFAULT, CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top,
		nullptr, nullptr, WindowClass::GetInstance(), this
	);

	if (!m_hWnd)
		throw HWND_LAST_EXCEPT();

	ShowWindow(m_hWnd, SW_SHOWDEFAULT);

	ImGuiImpl::ImGuiWindowInit(m_hWnd);

	Graphics::SetRenderFormat(DXGI_FORMAT_B8G8R8A8_UNORM);

	m_pGfx = std::make_unique<Graphics>(
		m_hWnd, static_cast<std::uint32_t>(width), static_cast<std::uint32_t>(height)
		);

	// Raw Input register
	RAWINPUTDEVICE rId = {};
	rId.usUsagePage = 1u;
	rId.usUsage = 2u;
	rId.dwFlags = 0;
	rId.hwndTarget = nullptr;

	if (!RegisterRawInputDevices(&rId, 1, sizeof(rId)))
		throw HWND_LAST_EXCEPT();
}

Window::~Window() {
	ImGuiImpl::ImGuiWindowQuit();
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
#ifdef _IMGUI
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	auto imIO = ImGui::GetIO();
#endif

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
	case WM_SIZE: {
		RECT clientRect = {};
		GetClientRect(m_hWnd, &clientRect);

		m_width = clientRect.right - clientRect.left;
		m_height = clientRect.bottom - clientRect.top;

		if(m_pGfx)
			m_pGfx->ResizeBuffer(m_width, m_height);

		if(!m_cursorEnabled)
			ConfineCursor();
		break;
	}
	case WM_ACTIVATE: {
		if (!m_cursorEnabled) {
			if (wParam & WA_ACTIVE || wParam & WA_CLICKACTIVE) {
				HideCursor();
				ConfineCursor();
			}
			else {
				ShowCursor();
				FreeCursor();
			}
		}
		break;
	}
	/************* KEYBOARD MESSAGES *************/
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN: {
#ifdef _IMGUI
		// Consume this message if ImGui wants to capture
		if (imIO.WantCaptureKeyboard)
			break;
#endif
		if ((wParam == VK_RETURN) && (lParam & (1 << 29)))
			ToggleFullScreenMode();

		if (!(lParam & 0x40000000) || m_kb.IsAutoRepeatEnabled()) // filters autoRepeat
			m_kb.OnKeyPressed(static_cast<unsigned char>(wParam));
		break;
	}
	case WM_KEYUP:
	case WM_SYSKEYUP: {
#ifdef _IMGUI
		// Consume this message if ImGui wants to capture
		if (imIO.WantCaptureKeyboard)
			break;
#endif

		m_kb.OnKeyReleased(static_cast<unsigned char>(wParam));
		break;
	}
	case WM_CHAR: {
#ifdef _IMGUI
		// Consume this message if ImGui wants to capture
		if (imIO.WantCaptureKeyboard)
			break;
#endif

		m_kb.OnChar(static_cast<char>(wParam));
		break;
	}
	/************* END KEYBOARD MESSAGES *************/
	/************* MOUSE MESSAGES *************/
	case WM_MOUSEMOVE: {
		if (!m_cursorEnabled && !m_mouse.IsInWindow()) {
			SetCapture(m_hWnd);
			m_mouse.OnMouseEnter();
			HideCursor();
			break;
		}

#ifdef _IMGUI
		// Consume this message if ImGui wants to capture
		if (imIO.WantCaptureMouse)
			break;
#endif

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
		SetForegroundWindow(m_hWnd);

#ifdef _IMGUI
		// Consume this message if ImGui wants to capture
		if (imIO.WantCaptureMouse)
			break;
#endif

		m_mouse.OnLeftPress();
		break;
	}
	case WM_LBUTTONUP: {
#ifdef _IMGUI
		// Consume this message if ImGui wants to capture
		if (imIO.WantCaptureMouse)
			break;
#endif

		m_mouse.OnLeftRelease();
		break;
	}
	case WM_MBUTTONDOWN: {
#ifdef _IMGUI
		// Consume this message if ImGui wants to capture
		if (imIO.WantCaptureMouse)
			break;
#endif

		m_mouse.OnMiddlePress();
		break;
	}
	case WM_MBUTTONUP: {
#ifdef _IMGUI
		// Consume this message if ImGui wants to capture
		if (imIO.WantCaptureMouse)
			break;
#endif

		m_mouse.OnMiddleRelease();
		break;
	}
	case WM_RBUTTONDOWN: {
#ifdef _IMGUI
		// Consume this message if ImGui wants to capture
		if (imIO.WantCaptureMouse)
			break;
#endif

		m_mouse.OnRightPress();
		break;
	}
	case WM_RBUTTONUP: {
#ifdef _IMGUI
		// Consume this message if ImGui wants to capture
		if (imIO.WantCaptureMouse)
			break;
#endif

		m_mouse.OnRightRelease();
		break;
	}
	case WM_MOUSEWHEEL: {
#ifdef _IMGUI
		// Consume this message if ImGui wants to capture
		if (imIO.WantCaptureMouse)
			break;
#endif

		int deltaWparam = GET_WHEEL_DELTA_WPARAM(wParam);

		m_mouse.OnWheelDelta(deltaWparam);
		break;
	}
	/************* END MOUSE MESSAGES *************/
	/************* RAW MOUSE MESSAGES *************/
	case WM_INPUT: {
		if (!m_mouse.IsRawEnabled())
			break;

		std::uint32_t size = 0;

		// Get raw data size with nullptr as buffer
		if (GetRawInputData(
			reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, nullptr, &size,
			sizeof(RAWINPUTHEADER)
		) == -1)
			break;

		m_rawInputBuffer.resize(size);

		// Get raw data by passing buffer
		if (GetRawInputData(
			reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT,
			m_rawInputBuffer.data(), &size,
			sizeof(RAWINPUTHEADER)
		) != size)
			break;

		auto& ri = reinterpret_cast<const RAWINPUT&>(*m_rawInputBuffer.data());
		if (ri.header.dwType == RIM_TYPEMOUSE &&
			(ri.data.mouse.lLastX != 0 || ri.data.mouse.lLastY != 0))
			m_mouse.OnMouseRawDelta(ri.data.mouse.lLastX, ri.data.mouse.lLastY);

		break;
	}
	/************* END RAW MOUSE MESSAGES *************/
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

void Window::ToggleFullScreenMode() {
	if (m_fullScreenMode) {
		SetWindowLong(m_hWnd, GWL_STYLE, m_windowStyle);

		SetWindowPos(
			m_hWnd,
			HWND_NOTOPMOST,
			m_windowRect.left,
			m_windowRect.top,
			m_windowRect.right - m_windowRect.left,
			m_windowRect.bottom - m_windowRect.top,
			SWP_FRAMECHANGED | SWP_NOACTIVATE
		);

		ShowWindow(m_hWnd, SW_NORMAL);
	}
	else {
		GetWindowRect(m_hWnd, &m_windowRect);

		SetWindowLong(
			m_hWnd, GWL_STYLE,
			m_windowStyle & ~(
				WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX |
				WS_SYSMENU)
		);

		RECT fullscreenWindowRect = m_pGfx->GetOutputDesc().DesktopCoordinates;

		SetWindowPos(
			m_hWnd,
			HWND_TOPMOST,
			fullscreenWindowRect.left,
			fullscreenWindowRect.top,
			fullscreenWindowRect.right,
			fullscreenWindowRect.bottom,
			SWP_FRAMECHANGED | SWP_NOACTIVATE
		);

		ShowWindow(m_hWnd, SW_MAXIMIZE);
	}

	m_fullScreenMode = !m_fullScreenMode;
}

void Window::EnableCursor() noexcept {
	m_cursorEnabled = true;

	ShowCursor();
	ImGuiImpl::EnableMouseInput();
	FreeCursor();
}

void Window::DisableCursor() noexcept {
	m_cursorEnabled = false;

	HideCursor();
	ImGuiImpl::DisableMouseInput();
	ConfineCursor();
}

void Window::HideCursor() noexcept {
	while (::ShowCursor(FALSE) >= 0);
}

void Window::ShowCursor() noexcept {
	while (::ShowCursor(TRUE) < 0);
}

void Window::ConfineCursor() noexcept {
	RECT rect = {};
	GetClientRect(m_hWnd, &rect);
	MapWindowPoints(m_hWnd, nullptr, reinterpret_cast<POINT*>(&rect), 2);
	ClipCursor(&rect);
}

void Window::FreeCursor() noexcept {
	ClipCursor(nullptr);
}

bool Window::IsCursorEnabled() const noexcept {
	return m_cursorEnabled;
}
