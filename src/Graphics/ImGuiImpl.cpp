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
		ID3D11Device* device, ID3D11DeviceContext* context
	) {
		ImGui_ImplDX11_Init(device, context);
	}

	void ImGuiDxQuit() {
		ImGui_ImplDX11_Shutdown();
	}

	void ImGuiBeginFrame() {
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiEndFrame() {
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
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
		ID3D11Device* device, ID3D11DeviceContext* context
	) {}

	void ImGuiDxQuit() {}

	void ImGuiBeginFrame() {}

	void ImGuiEndFrame() {}

	void ImGuiInitContext() {}

	void ImGuiDestroyContext() {}

	void ImGuiRenderSimulationSlider(float& speedFactor, bool isPaused) {}
}

#endif
