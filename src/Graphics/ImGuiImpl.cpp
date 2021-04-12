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
}

#endif
