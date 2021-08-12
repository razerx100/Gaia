#ifndef __APP_HPP__
#define __APP_HPP__
#include <Window.hpp>
#include <Timer.hpp>
#include <vector>
#include <Camera.hpp>
#include <Model.hpp>
#include <Quad.hpp>
#include <TestObject.hpp>
#include <Test/Box.hpp>

class App {
public:
	App();
	~App() = default;

	int Go();

private:
	void DoFrame();

	// Key events
	void InputLoop(float deltaTime) noexcept;
	void ToggleCursor(std::uint8_t keyCode) noexcept;
	void CameraMovement(float deltaTime) noexcept;

private:
	Window m_wnd;
	Timer m_timer;
	Camera m_camera;
	TestObject<Model> m_pNano;
	TestObject<Model> m_pNano2;
	TestObject<Box> m_pBox;
};
#endif