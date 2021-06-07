#include <App.hpp>
#include <random>
#include <Box.hpp>
#include <Cylinder.hpp>
#include <Pyramid.hpp>
#include <Tex2DBox.hpp>
#include <Model.hpp>
#include <algorithm>
#include <GDIPlusManager.hpp>
#include <ImGuiImpl.hpp>

#ifdef _IMGUI
#include <ImGuiMan.hpp>
ImGuiMan ImGuiMan::s_initObj;
#endif

GDIPlusManager gdim;

std::unique_ptr<Light> App::s_Light;

App::App()
	:
	m_Wnd(1980, 1080, "DirectX12 Window"),
	m_SpeedFactor(1.0f) {

	Drawable::SetShaderPath();

	s_Light = std::make_unique<Light>(m_Wnd.GetGfx(), 0.4f);

	class Factory {
	public:
		Factory(Graphics& gfx)
			: m_gfx(gfx) {}

		std::unique_ptr<Drawable> operator()() {
			DirectX::XMFLOAT4 mat = {
				cdist(rng), cdist(rng), cdist(rng), 10.f
			};

			switch (sdist(rng)) {
			case 0:
				return std::make_unique<Box>(
					m_gfx, rng, adist, ddist,
					odist, rdist, bdist, mat
					);

			case 1:
				return std::make_unique<Cylinder>(
					m_gfx, rng, adist, ddist,
					odist, rdist, tdist, mat
					);

			case 2:
				return std::make_unique<Pyramid>(
					m_gfx, rng, adist, ddist,
					odist, rdist, tdist
					);

			case 3:
				return std::make_unique<Tex2DBox>(
					m_gfx, rng, adist, ddist,
					odist, rdist
					);

			case 4:
				return std::make_unique<Model>(
					m_gfx, rng, adist, ddist,
					odist, rdist, mat, 1.5f
					);

			default:
				return {};
			}
		}

	private:
		Graphics& m_gfx;

		std::mt19937 rng{ std::random_device{}() };
		std::uniform_int_distribution<int> sdist{ 0, 4 };
		std::uniform_int_distribution<int> tdist{ 3, 30 };
		std::uniform_real_distribution<float> adist{ 0.0f, DirectX::XM_PI * 2.0f };
		std::uniform_real_distribution<float> ddist{ 0.0f, DirectX::XM_PI * 0.5f };
		std::uniform_real_distribution<float> odist{ 0.0f, DirectX::XM_PI * 0.08f };
		std::uniform_real_distribution<float> rdist{ 6.0f, 20.0f };
		std::uniform_real_distribution<float> bdist{ 0.4f, 3.0f };
		std::uniform_real_distribution<float> cdist{ 0.0f, 1.0f };
	};

	Factory f(m_Wnd.GetGfx());
	m_pDrawables.reserve(s_nDrawables);

	std::generate_n(std::back_inserter(m_pDrawables), s_nDrawables, f);

	Camera::SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 9.0f / 16.0f, 0.5f, 60.0f));

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

	for (auto& drawable : m_pDrawables) {
		drawable->Update(m_Wnd.m_kb.IsKeyPressed(VK_SPACE) ? 0.0f : deltaTime);
		drawable->Draw(m_Wnd.GetGfx());
	};

	s_Light->Draw(m_Wnd.GetGfx());

	ImGuiImpl::ImGuiRenderSimulationSlider(m_SpeedFactor, m_Wnd.m_kb.IsKeyPressed(VK_SPACE));

	m_Camera.ControlWindow();

	s_Light->ImGuiLightSlider();

	m_Wnd.GetGfx().EndFrame();
}

Light* App::GetLight() noexcept {
	return s_Light.get();
}
