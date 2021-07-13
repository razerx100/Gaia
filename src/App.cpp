#include <App.hpp>
#include <random>
#include <algorithm>
#include <GDIPlusManager.hpp>
#include <ImGuiImpl.hpp>
#include <BindableCodex.hpp>
#include <Bindable.hpp>
#include <SolidSphere.hpp>

#ifdef _IMGUI
#include <ImGuiMan.hpp>
ImGuiMan ImGuiMan::s_initObj;
#endif

GDIPlusManager gdim;
std::string App::s_shaderPath;
// Codex instance defined here so it's destroyed last
Codex Codex::s_Instance;
Light App::s_light;
ImGuiImpl::Position pos;

App::App()
	:
	m_wnd(1920, 1080, "Gaia") {

	SetShaderPath();

	Camera::SetCameraInstance(&m_camera);
	Camera::SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 1080.0f / 1920.0f, 0.5f, 60.0f));

	s_light.Init(m_wnd.GetGfx(), 0.4f);

	m_pNano = std::make_unique<Model>(m_wnd.GetGfx(), "models\\nano_textured\\nanosuit.obj");

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
	m_wnd.GetGfx().BeginFrame(0.07f, 0.0f, 0.12f);

	s_light.Update();
	pos.Update();
	InputLoop(deltaTime);

	s_light.Draw(m_wnd.GetGfx());
	m_pNano->Draw(m_wnd.GetGfx(), pos.GetTransform());

	ImGuiImpl::ImGuiRenderFPSCounter();
	m_camera.ControlWindow();
	ImGuiImpl::ImGuiModelControl(pos);
	s_light.ImGuiLightSlider();

	m_wnd.GetGfx().EndFrame();
}

void App::SetShaderPath() noexcept {
	char path[MAX_PATH];
	GetModuleFileNameA(nullptr, path, MAX_PATH);
	s_shaderPath = path;
	for (int i = static_cast<int>(s_shaderPath.size() - 1); s_shaderPath[i] != '\\'; i--)
		s_shaderPath.pop_back();

	s_shaderPath.append("shaders\\");
}

std::string App::GetShaderPath() noexcept {
	return s_shaderPath;
}

Light* App::GetLight() noexcept {
	return &s_light;
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
