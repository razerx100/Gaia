#include <App.hpp>
#include <random>
#include <Melon.hpp>
#include <Box.hpp>
#include <Pyramid.hpp>
#include <algorithm>
#include <Sheet.hpp>
#include <SkinnedBox.hpp>
#include <GDIPlusManager.hpp>

GDIPlusManager gdipm;

App::App()
	: m_wnd(1980, 1080, "DirectX11 Window") {

	Drawable::SetShaderPath();

	class Factory {
	public:
		Factory(Graphics& gfx)
			: m_gfx(gfx) {}

		std::unique_ptr<Drawable> operator()() {
			switch (typedist(rng))
			{
			case 0:
				return std::make_unique<Pyramid>(
					m_gfx, rng, adist, ddist,
					odist, rdist
					);
			case 1:
				return std::make_unique<Box>(
					m_gfx, rng, adist, ddist,
					odist, rdist, bdist
					);
			case 2:
				return std::make_unique<Melon>(
					m_gfx, rng, adist, ddist,
					odist, rdist, longdist, latdist
					);
			case 3:
				return std::make_unique<Sheet>(
					m_gfx, rng, adist, ddist,
					odist, rdist
					);
			case 4:
				return std::make_unique<SkinnedBox>(
					m_gfx, rng, adist, ddist,
					odist, rdist
					);
			default:
				assert(false && "bad drawable type in factory");
				return {};
			}
		}

	private:
		Graphics& m_gfx;

		std::mt19937 rng{ std::random_device{}() };
		std::uniform_real_distribution<float> adist{ 0.0f, DirectX::XM_PI * 2.0f };
		std::uniform_real_distribution<float> ddist{ 0.0f, DirectX::XM_PI * 0.5f };
		std::uniform_real_distribution<float> odist{ 0.0f, DirectX::XM_PI * 0.08f };
		std::uniform_real_distribution<float> rdist{ 6.0f, 20.0f };
		std::uniform_real_distribution<float> bdist{ 0.4f, 3.0f };
		std::uniform_int_distribution<int> latdist{ 5, 20 };
		std::uniform_int_distribution<int> longdist{ 10, 40 };
		std::uniform_int_distribution<int> typedist{ 0, 4 };
	};

	Factory f(m_wnd.GetGfx());
	m_drawables.reserve(nDrawables);

	std::generate_n(std::back_inserter(m_drawables), nDrawables, f);
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

	for (auto& da : m_drawables) {
		da->Update(m_wnd.GetGfx(), deltaTime);
		da->Draw(m_wnd.GetGfx());
	}
	m_wnd.GetGfx().EndFrame();
}