#include <ImGuiImpl.hpp>

#ifdef _IMGUI
namespace ImGuiImpl {
	void ImGuiWindowInit(void* hwnd) {
		ImGui_ImplWin32_Init(hwnd);
	}

	void ImGuiWindowQuit() {
		ImGui_ImplWin32_Shutdown();
	}

	void ImGuiDxInit(
		ID3D12Device* device, int bufferCount,
		DXGI_FORMAT rtvFormat, HeapMan* heapman
	) {
		ImGui_ImplDX12_Init(device, bufferCount, rtvFormat, heapman);
	}

	void ImGuiDxQuit() {
		ImGui_ImplDX12_Shutdown();
	}

	void ImGuiBeginFrame() {
		ImGui_ImplDX12_GetSRVHeapData();
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiEndFrame(ID3D12GraphicsCommandList* commandlist) {
		ImGui::Render();
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandlist);

		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault(nullptr, (void*)commandlist);
		}
	}

	void ImGuiInitContext() {
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();

		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	}

	void ImGuiDestroyContext() {
		ImGui::DestroyContext();
	}

	void ImGuiRenderSimulationSlider(float& speedFactor, bool isPaused) {
		if (ImGui::Begin("Simulation Speed")) {
			ImGui::SliderFloat("Speed Factor", &speedFactor, 0.0f, 4.0f);
			ImGui::Text(
				"Application average %.3f ms/frame (%.1f FPS)",
				1000.0f / ImGui::GetIO().Framerate,
				ImGui::GetIO().Framerate
			);
			ImGui::Text(
				"Status: %s", isPaused ? "PAUSED" : "RUNNING"
			);
		}

		ImGui::End();
	}

	void ImGuiRenderCameraControl(
		float& r, float& theta, float& phi,
		float& roll, float& pitch, float& yaw,
		std::function<void()> resetButton
	) {
		if (ImGui::Begin("Camera")) {
			ImGui::Text("Position");
			ImGui::SliderFloat("R", &r, 0.0f, 80.0f, "%.1f");
			ImGui::SliderAngle("X", &theta, -180.0f, 180.0f);
			ImGui::SliderAngle("Y", &phi, -89.0f, 89.0f);
			ImGui::Text("Orientation");
			ImGui::SliderAngle("Roll", &roll, -180.0f, 180.0f);
			ImGui::SliderAngle("Pitch", &pitch, -180.0f, 180.0f);
			ImGui::SliderAngle("Yaw", &yaw, -180.0f, 180.0f);
			if (ImGui::Button("Reset"))
			{
				resetButton();
			}
		}

		ImGui::End();
	}

	void ImGuiLightSlider(float& x, float& y, float& z) {
		if (ImGui::Begin("Light")) {
			ImGui::Text("Position");
			ImGui::SliderFloat("X", &x, -1.0f, 1.0f, "%.1f");
			ImGui::SliderFloat("Y", &y, -1.0f, 1.0f, "%.1f");
			ImGui::SliderFloat("Z", &z, -1.0f, 1.0f, "%.1f");
		}

		ImGui::End();
	}
}

#else

namespace ImGuiImpl {
	void ImGuiWindowInit(void* hwnd) {}

	void ImGuiWindowQuit() {}

	void ImGuiDxInit(
		ID3D12Device* device, int bufferCount,
		DXGI_FORMAT rtvFormat, HeapMan* heapman
	) {}

	void ImGuiDxQuit() {}

	void ImGuiBeginFrame() {}

	void ImGuiEndFrame(ID3D12GraphicsCommandList* commandlist) {}

	void ImGuiInitContext() {}

	void ImGuiDestroyContext() {}

	void ImGuiRenderSimulationSlider(float& speedFactor, bool isPaused) {}

	void ImGuiRenderCameraControl(
		float& r, float& theta, float& phi,
		float& roll, float& pitch, float& yaw,
		std::function<void()> resetButton
	) {}

	void ImGuiLightSlider(float& x, float& y, float& z) {}
}

#endif
