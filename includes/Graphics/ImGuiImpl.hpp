#ifndef __IMGUI_IMPL_HPP__
#define __IMGUI_IMPL_HPP__
#include <ImGuiAll.hpp>
#include <CleanWin.hpp>
#include <d3d12.h>
#include <HeapMan.hpp>
#include <Light.hpp>
#include <functional>
#include <DirectXMath.h>

struct Position;

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

	void EnableMouseInput() noexcept;
	void DisableMouseInput() noexcept;

	// My Widgets
	void ImGuiRenderFPSCounter();

	void ImGuiRenderCameraControl(
		DirectX::XMFLOAT3& cameraPosition,
		float& pitch,
		float& yaw,
		std::function<void()> resetButton
	);

	void ImGuiModelControl(
		const std::string& name,
		Position& position
	);

	void ImGuiLightSlider(
		LightData& data,
		std::function<void()> resetButton
	);
}
#endif
