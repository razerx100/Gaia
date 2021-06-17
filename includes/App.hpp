#ifndef __APP_HPP__
#define __APP_HPP__
#include <Window.hpp>
#include <Timer.hpp>
#include <vector>
#include <Camera.hpp>
#include <Light.hpp>
#include <Model.hpp>
#include <Box.hpp>

struct Position {
	float roll = 0.0f;
	float pitch = -3.1f;
	float yaw = 0.0f;
	float x = 0.0f;
	float y = -8.5f;
	float z = 0.0f;
};

class App {
public:
	App();
	~App() = default;

	int Go();

	static Light* GetLight() noexcept;
	static std::wstring GetShaderPath() noexcept;

private:
	void DoFrame();

	static void SetShaderPath() noexcept;

private:
	Window m_Wnd;
	Timer m_Timer;
	Camera m_Camera;
	std::unique_ptr<Model> m_pNano;
	std::unique_ptr<Box> m_pBox;

	float m_SpeedFactor;

	static std::wstring s_ShaderPath;
	static std::unique_ptr<Light> s_Light;

	Position pos;
};
#endif