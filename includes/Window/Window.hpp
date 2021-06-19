#ifndef __WINDOW_HPP__
#define __WINDOW_HPP__
#include <CleanWin.hpp>
#include <Keyboard.hpp>
#include <Mouse.hpp>
#include <Graphics.hpp>
#include <optional>
#include <memory>

class Window {
private:

	class WindowClass {
	public:
		static const char* GetName() noexcept;
		static HINSTANCE GetInstance() noexcept;

	private:
		WindowClass() noexcept;
		~WindowClass();
		WindowClass(const WindowClass&) = delete;
		WindowClass& operator=(const WindowClass&) = delete;
		static constexpr const char* wndClassName = "Direct3D Engine Window";
		static WindowClass s_wndClass;
		HINSTANCE m_hInst;
	};

public:
	Window(int width, int height, const char* name);
	~Window();
	Window(const Window&) = delete;

	Window& operator=(const Window&) = delete;

	void SetTitle(const std::string& title);

	Graphics& GetGfx() const;
	bool IsCursorEnabled() const noexcept;

	void EnableCursor() noexcept;
	void DisableCursor() noexcept;
	void ConfineCursor() noexcept;
	void FreeCursor() noexcept;

	static std::optional<int> ProcessMessages();

private:
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT CALLBACK HandleMsgWrap(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	void ToggleFullScreenMode();
	void HideCursor() noexcept;
	void ShowCursor() noexcept;

public:
	Keyboard m_kb;
	Mouse m_mouse;

private:
	int m_width;
	int m_height;
	HWND m_hWnd;
	std::unique_ptr<Graphics> m_pGfx;

	bool m_fullScreenMode;
	std::uint32_t m_windowStyle;
	RECT m_windowRect;

	bool m_cursorEnabled;
	std::vector<std::uint8_t> m_rawInputBuffer;
};

#endif