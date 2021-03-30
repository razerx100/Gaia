#include <App.hpp>
#include <sstream>
#include <iomanip>

App::App()
	: m_wnd(1980, 1080, "DirectX11 Window") {}

int App::Go() {
	while (true) {
		if (const auto ecode = Window::ProcessMessages())
			return *ecode;

		DoFrame();
	}
}

void App::DoFrame() {
	const float c = sin(m_timer.Peek()) / 2.0f + 0.5f;
	m_wnd.GetGfx().ClearBuffer(c, c, 1.0f);
	/*m_wnd.GetGfx().DrawTriangle(
		m_timer.Peek(),
		m_wnd.m_mouse.GetPosX() / 640.0f - 1,
		-m_wnd.m_mouse.GetPosY() / 360.0f + 1
	);*/
	m_wnd.GetGfx().EndFrame();
}