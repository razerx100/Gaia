#include <App.hpp>
#include <random>

App::App()
	: m_wnd(1980, 1080, "DirectX11 Window") {

	Drawable::SetShaderPath();

	std::mt19937 rng(std::random_device{}());

	std::uniform_real_distribution<float> adist(0.0f, 3.1416f * 2.0f);
	std::uniform_real_distribution<float> ddist(0.0f, 3.1416f * 2.0f);
	std::uniform_real_distribution<float> odist(0.0f, 3.1416f * 0.3f);
	std::uniform_real_distribution<float> rdist(6.0f, 20.0f);
	for(int i = 0; i < 300; i++)
		m_cubes.emplace_back(std::make_unique<D3Object>(m_wnd.GetGfx(),
			rng, adist, ddist, odist, rdist
			));
}

App::~App(){}

int App::Go() {
	while (true) {
		if (const auto ecode = Window::ProcessMessages())
			return *ecode;

		DoFrame();
	}
}

void App::DoFrame() {
	const float deltaTime = m_timer.Mark();
	m_wnd.GetGfx().ClearBuffer(0.07f, 0.0f, 0.12f);

	for (auto& cube : m_cubes) {
		cube->Update(m_wnd.GetGfx(), deltaTime);
		cube->Draw(m_wnd.GetGfx());
	}
	m_wnd.GetGfx().EndFrame();
}