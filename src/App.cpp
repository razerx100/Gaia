#include <App.hpp>
#include <random>

App::App()
	: m_wnd(1980, 1080, "DirectX11 Window") {

	std::mt19937 rng(std::random_device{}());

	std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

	for(int i = 0; i < 5000; i++)
		m_cubes.emplace_back(std::make_unique<Cube>(m_wnd.GetGfx(),
			dist(rng),
			dist(rng)
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
	const float c = sin(m_timer.Peek()) / 2.0f + 0.5f;
	m_wnd.GetGfx().ClearBuffer(0.07f, 0.0f, 0.12f);

	for (auto& cube : m_cubes) {
		cube->Update(m_wnd.GetGfx(), c);
		cube->Draw(m_wnd.GetGfx());
	}
	m_wnd.GetGfx().EndFrame();
}