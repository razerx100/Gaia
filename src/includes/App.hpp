#ifndef __APP_HPP__
#define __APP_HPP__
#include <Window.hpp>

class App {
public:
	App();

	int Go();

private:
	void DoFrame();

private:
	Window m_wnd;

};
#endif