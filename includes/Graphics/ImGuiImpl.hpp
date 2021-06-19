#ifndef __IMGUI_IMPL_HPP__
#define __IMGUI_IMPL_HPP__
#include <ImGuiAll.hpp>
#include <CleanWin.hpp>
#include <d3d12.h>
#include <HeapMan.hpp>
#include <Light.hpp>
#include <functional>
#include <DirectXMath.h>

namespace ImGuiImpl {
	class Position {
	public:
		float roll = 0.0f;
		float pitch = -3.1f;
		float yaw = 0.0f;
		float x = 0.0f;
		float y = -8.5f;
		float z = 0.0f;

		void Update() noexcept {
			m_transform =
				DirectX::XMMatrixRotationRollPitchYaw(
					roll, pitch, yaw
				)
				*
				DirectX::XMMatrixTranslation(
					x, y, z
				);
		}

		DirectX::XMMATRIX GetTransform() const noexcept {
			return m_transform;
		}

	private:
		DirectX::XMMATRIX m_transform;
	};

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
	void ImGuiRenderSimulationSlider(float& speedFactor, bool isPaused);

	void ImGuiRenderCameraControl(
		DirectX::XMFLOAT3& cameraPosition,
		float& pitch,
		float& yaw,
		std::function<void()> resetButton
	);

	void ImGuiModelControl(
		Position& position
	);

	void ImGuiLightSlider(
		LightData& data,
		std::function<void()> resetButton
	);
}
#endif
