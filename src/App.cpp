#include <App.hpp>
#include <random>
#include <algorithm>
#include <GDIPlusManager.hpp>
#include <ImGuiImpl.hpp>
#include <BindableCodex.hpp>
#include <Bindable.hpp>
#include <SolidSphere.hpp>
#include <Utilities.hpp>
#include <Light.hpp>

#ifdef _IMGUI
#include <ImGuiMan.hpp>
ImGuiMan ImGuiMan::s_initObj;
#endif

GDIPlusManager gdim;
// Codex instance defined here so it's destroyed last
Codex Codex::s_Instance;

App::App()
	:
	m_wnd(1920, 1080, "Gaia") {

	GUtil::ShaderPath::Set();
	m_wnd.GetGfx().SetBGColor(0.07f, 0.0f, 0.12f);

	Camera::SetCameraInstance(&m_camera);
	Camera::SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 1080.0f / 1920.0f, 0.5f, 60.0f));

	Light::GetLight()->Init(m_wnd.GetGfx(), 0.4f);

	m_pNano.Init(
		m_wnd.GetGfx(), "NanoSuit1",
		6.0f, -8.5f, 0.0f,
		"models\\nano_textured\\nanosuit.obj"
		);
	m_pNano2.Init(
		m_wnd.GetGfx(), "NanoSuit2",
		-6.0f, -8.5f, 0.0f,
		"models\\nano_textured\\nanosuit.obj"
		);
	m_pWall.Init(
		m_wnd.GetGfx(),
		"Wall",
		0.0f, 3.0f, -8.5f,
		"models\\brick_wall\\brick_wall.obj"
		);

	m_wnd.GetGfx().InitialGPUSetup();
}

int App::Go() {

	while (true) {
		if (const auto ecode = Window::ProcessMessages())
			return *ecode;

		DoFrame();
		m_wnd.GetGfx().PresentFrame();
	}
}

void App::DoFrame() {
	const float deltaTime = m_timer.Mark();
	m_wnd.GetGfx().BeginFrame();

	Light::UpdateLights(m_wnd.GetGfx());
	InputLoop(deltaTime);

	m_pNano.Draw(m_wnd.GetGfx());
	m_pNano2.Draw(m_wnd.GetGfx());
	m_pWall.Draw(m_wnd.GetGfx());

	ImGuiImpl::ImGuiRenderFPSCounter();
	m_camera.ControlWindow();

	m_wnd.GetGfx().EndFrame();
}

void App::InputLoop(float deltaTime) noexcept {
	while (auto event = m_wnd.m_kb.ReadKey()) {
		if (!event->IsPress())
			continue;

		std::uint8_t keyCode = event->GetCode();

		ToggleCursor(keyCode);
	}

	CameraMovement(deltaTime);
}

void App::ToggleCursor(std::uint8_t keyCode) noexcept {
	if (keyCode == VK_ESCAPE) {
		if (m_wnd.IsCursorEnabled()) {
			m_wnd.DisableCursor();
			m_wnd.m_mouse.EnableRaw();
		}
		else {
			m_wnd.EnableCursor();
			m_wnd.m_mouse.DisableRaw();
		}
	}
}

void App::CameraMovement(float deltaTime) noexcept {
	if (!m_wnd.IsCursorEnabled()) {
		if (m_wnd.m_kb.IsKeyPressed('W'))
			m_camera.Translate({ 0.0f, 0.0f, deltaTime });

		if (m_wnd.m_kb.IsKeyPressed('A'))
			m_camera.Translate({ -deltaTime, 0.0f, 0.0f });

		if (m_wnd.m_kb.IsKeyPressed('S'))
			m_camera.Translate({ 0.0f, 0.0f, -deltaTime });

		if (m_wnd.m_kb.IsKeyPressed('D'))
			m_camera.Translate({ deltaTime, 0.0f, 0.0f });

		if (m_wnd.m_kb.IsKeyPressed('R'))
			m_camera.Translate({ 0.0f, deltaTime, 0.0f });

		if (m_wnd.m_kb.IsKeyPressed('F'))
			m_camera.Translate({ 0.0f, -deltaTime, 0.0f });

		while (auto rawDelta = m_wnd.m_mouse.ReadRawDelta())
			m_camera.Rotate(
				static_cast<float>(rawDelta->x), static_cast<float>(rawDelta->y)
			);
	}
}
