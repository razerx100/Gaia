#ifndef __APP_HPP__
#define __APP_HPP__
#include <Window.hpp>
#include <Timer.hpp>
#include <vector>
#include <Camera.hpp>
#include <Light.hpp>
#include <Model.hpp>

class App {
public:
	App();
	~App() = default;

	int Go();

	static Light* GetLight() noexcept;
	static std::wstring GetShaderPath() noexcept;

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

	float m_speedFactor;

	static std::wstring s_shaderPath;
	static std::unique_ptr<Light> s_light;
};
#endif