#include <App.hpp>
#include <random>
#include <algorithm>
#include <GDIPlusManager.hpp>
#include <ImGuiImpl.hpp>

#ifdef _IMGUI
#include <ImGuiMan.hpp>
ImGuiMan ImGuiMan::s_initObj;
#endif

GDIPlusManager gdim;
std::wstring App::s_shaderPath;

std::unique_ptr<Light> App::s_light;

ImGuiImpl::Position pos;

App::App()
	:
	m_wnd(1920, 1080, "DirectX12 Window"),
	m_speedFactor(1.0f) {

	SetShaderPath();

	Camera::SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 1080.0f / 1920.0f, 0.5f, 60.0f));

	s_light = std::make_unique<Light>(m_wnd.GetGfx(), 0.4f);

	m_pNano = std::make_unique<Model>(m_wnd.GetGfx(), "models\\nanosuit.obj");

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
	const float deltaTime = m_timer.Mark() * m_speedFactor;
	m_wnd.GetGfx().BeginFrame(0.07f, 0.0f, 0.12f);

	m_camera.Update();
	s_light->Update();
	pos.Update();
	m_wnd.ToggleCursor();

	s_light->Draw(m_wnd.GetGfx());
	m_pNano->Draw(m_wnd.GetGfx(), pos.GetTransform());

	ImGuiImpl::ImGuiRenderSimulationSlider(m_speedFactor, m_wnd.m_kb.IsKeyPressed(VK_SPACE));
	m_camera.ControlWindow();
	ImGuiImpl::ImGuiModelControl(pos);
	s_light->ImGuiLightSlider();

	m_wnd.GetGfx().EndFrame();
}

void App::SetShaderPath() noexcept {
	wchar_t path[MAX_PATH];
	GetModuleFileNameW(nullptr, path, MAX_PATH);
	s_shaderPath = path;
	for (int i = static_cast<int>(s_shaderPath.size() - 1); s_shaderPath[i] != L'\\'; i--)
		s_shaderPath.pop_back();

	s_shaderPath.append(L"shaders\\");
}

std::wstring App::GetShaderPath() noexcept {
	return s_shaderPath;
}

Light* App::GetLight() noexcept {
	return s_light.get();
}
