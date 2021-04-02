#ifndef __APP_HPP__
#define __APP_HPP__
#include <Window.hpp>
#include <Timer.hpp>
#include <Drawable.hpp>

class App {
public:
	App();
	~App();

	int Go();

private:
	void DoFrame();

private:
	Window m_wnd;
	Timer m_timer;

	std::vector<std::unique_ptr<Drawable>> m_drawables;

	static constexpr std::uint64_t nDrawables = 380u;
};
#endif