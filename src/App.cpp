#include <App.hpp>
#include <random>
#include <Box.hpp>
#include <algorithm>
#include <GDIPlusManager.hpp>
#include <ImGuiImpl.hpp>

#ifdef _IMGUI
#include <ImGuiMan.hpp>
ImGuiMan ImGuiMan::s_initObj;
#endif

GDIPlusManager gdim;

App::App()
	: m_wnd(1980, 1080, "DirectX12 Window"), m_speedFactor(1.0f) {

	Drawable::SetShaderPath();

	class Factory {
	public:
		Factory(Graphics& gfx)
			: m_gfx(gfx) {}

		std::unique_ptr<Drawable> operator()() {
			return std::make_unique<Box>(
				m_gfx, rng, adist, ddist,
				odist, rdist, bdist
				);
		}

	private:
		Graphics& m_gfx;

		std::mt19937 rng{ std::random_device{}() };
		std::uniform_real_distribution<float> adist{ 0.0f, DirectX::XM_PI * 2.0f };
		std::uniform_real_distribution<float> ddist{ 0.0f, DirectX::XM_PI * 0.5f };
		std::uniform_real_distribution<float> odist{ 0.0f, DirectX::XM_PI * 0.08f };
		std::uniform_real_distribution<float> rdist{ 6.0f, 20.0f };
		std::uniform_real_distribution<float> bdist{ 0.4f, 3.0f };
	};

	Factory f(m_wnd.GetGfx());
	m_drawables.reserve(nDrawables);

	std::generate_n(std::back_inserter(m_drawables), nDrawables, f);

	Camera::SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 9.0f / 16.0f, 0.5f, 40.0f));

	m_wnd.GetGfx().InitialGPUSetup();
}

int App::Go() {
	while (true) {
		if (const auto ecode = Window::ProcessMessages())
			return *ecode;

		DoFrame();
	}
}

void App::DoFrame() {
	const float deltaTime = m_timer.Mark() * m_speedFactor;
	m_wnd.GetGfx().BeginFrame(0.07f, 0.0f, 0.12f);
	m_camera.Update();

	for (auto& da : m_drawables) {
		da->Update(m_wnd.m_kb.IsKeyPressed(VK_SPACE) ? 0.0f : deltaTime);
		da->Draw(m_wnd.GetGfx());
	}

	ImGuiImpl::ImGuiRenderSimulationSlider(m_speedFactor, m_wnd.m_kb.IsKeyPressed(VK_SPACE));
	m_camera.ControlWindow();

	m_wnd.GetGfx().EndFrame();
}
