#ifndef __IMGUI_IMPL_HPP__
#define __IMGUI_IMPL_HPP__
#include <ImGuiAll.hpp>
#include <CleanWin.hpp>
#include <d3d12.h>
#include <HeapMan.hpp>
#include <functional>

namespace ImGuiImpl {
	void ImGuiWindowInit(void* hwnd);

	void ImGuiWindowQuit();

	void ImGuiDxInit(
		ID3D12Device* device, int bufferCount,
		DXGI_FORMAT rtvFormat, HeapMan* heapman
	);

	void ImGuiDxQuit();

	void ImGuiBeginFrame();

	void ImGuiEndFrame(ID3D12GraphicsCommandList* commandlist);

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
