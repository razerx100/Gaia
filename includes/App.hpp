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
	Window m_Wnd;
	Timer m_Timer;
	Camera m_Camera;

	std::vector<std::unique_ptr<Drawable>> m_pDrawables;

	static std::unique_ptr<Light> s_Light;

	static constexpr std::uint64_t s_nDrawables = 380u;
	float m_SpeedFactor;
};
#endif