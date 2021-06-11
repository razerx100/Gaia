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
std::wstring App::s_ShaderPath;

std::unique_ptr<Light> App::s_Light;

App::App()
	:
	m_Wnd(1980, 1080, "DirectX12 Window"),
	m_SpeedFactor(1.0f) {

	SetShaderPath();

	s_Light = std::make_unique<Light>(m_Wnd.GetGfx(), 0.4f);

	Camera::SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 9.0f / 16.0f, 0.5f, 60.0f));

	m_pNano = std::make_unique<Model>(m_Wnd.GetGfx(), "models\\nanosuit.obj");

	m_Wnd.GetGfx().InitialGPUSetup();
}

int App::Go() {

	while (true) {
		if (const auto ecode = Window::ProcessMessages())
			return *ecode;

		DoFrame();
		m_Wnd.GetGfx().PresentFrame();
	}
}

void App::DoFrame() {
	const float deltaTime = m_Timer.Mark() * m_SpeedFactor;
	m_Wnd.GetGfx().BeginFrame(0.07f, 0.0f, 0.12f);
	m_Camera.Update();

	s_Light->Update();

	s_Light->Draw(m_Wnd.GetGfx());

	DirectX::XMMATRIX transform =
		DirectX::XMMatrixRotationRollPitchYaw(
			pos.roll, pos.pitch, pos.yaw
		)
		*
		DirectX::XMMatrixTranslation(
			pos.x, pos.y, pos.z
		);

	m_pNano->Draw(m_Wnd.GetGfx(), transform);

	ImGuiImpl::ImGuiRenderSimulationSlider(m_SpeedFactor, m_Wnd.m_kb.IsKeyPressed(VK_SPACE));

	m_Camera.ControlWindow();

	ImGuiImpl::ImGuiModelControl(pos);

	s_Light->ImGuiLightSlider();

	m_Wnd.GetGfx().EndFrame();
}

void App::SetShaderPath() noexcept {
	wchar_t path[MAX_PATH];
	GetModuleFileNameW(nullptr, path, MAX_PATH);
	s_ShaderPath = path;
	for (int i = static_cast<int>(s_ShaderPath.size() - 1); s_ShaderPath[i] != L'\\'; i--)
		s_ShaderPath.pop_back();

	s_ShaderPath.append(L"shaders\\");
}

std::wstring App::GetShaderPath() noexcept {
	return s_ShaderPath;
}

Light* App::GetLight() noexcept {
	return s_Light.get();
}


