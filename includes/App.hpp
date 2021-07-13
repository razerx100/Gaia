#ifndef __APP_HPP__
#define __APP_HPP__
#include <Window.hpp>
#include <Timer.hpp>
#include <vector>
#include <Camera.hpp>
#include <Model.hpp>
#include <Light.hpp>

class App {
public:
	App();
	~App() = default;

	int Go();

	static Light* GetLight() noexcept;
	static std::string GetShaderPath() noexcept;

private:
	void DoFrame();

	// Key events
	void InputLoop(float deltaTime) noexcept;
	void ToggleCursor(std::uint8_t keyCode) noexcept;
	void CameraMovement(float deltaTime) noexcept;

	static void SetShaderPath() noexcept;

private:
	Window m_wnd;
	Timer m_timer;
	Camera m_camera;
	std::unique_ptr<Model> m_pNano;

	static std::string s_shaderPath;
	static Light s_light;
};
#endif