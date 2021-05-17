#ifndef __APP_HPP__
#define __APP_HPP__
#include <Window.hpp>
#include <Drawable.hpp>
#include <Timer.hpp>
#include <vector>
#include <Camera.hpp>
#include <Light.hpp>

class App {
public:
	App();
	~App() = default;

	int Go();

	static Light* GetLight() noexcept;

private:
	void DoFrame();

private:
	Window m_wnd;
	Timer m_timer;
	Camera m_camera;

	std::vector<std::unique_ptr<Drawable>> m_drawables;

	static std::unique_ptr<Light> s_light;

	static constexpr std::uint64_t nDrawables = 380u;
	float m_speedFactor;
};
#endif