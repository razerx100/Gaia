#ifndef __IMGUI_IMPL_HPP__
#define __IMGUI_IMPL_HPP__
#include <ImGuiAll.hpp>
#include <CleanWin.hpp>
#include <d3d11.h>
#include <functional>

namespace ImGuiImpl {
	void ImGuiWindowInit(void* hwnd);

	void ImGuiWindowQuit();

	void ImGuiDxInit(
		ID3D11Device* device, ID3D11DeviceContext * context
	);

	void ImGuiDxQuit();

	void ImGuiBeginFrame();

	void ImGuiEndFrame();

	void ImGuiInitContext();

	void ImGuiDestroyContext();

	// My Widgets
	void ImGuiRenderSimulationSlider(float& speedFactor, bool isPaused);

	void ImGuiRenderCameraControl(
		float& r, float& theta, float& phi,
		float& roll, float& pitch, float& yaw,
		std::function<void()> resetButton
	);
}
#endif
