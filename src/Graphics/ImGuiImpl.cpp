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

	void ImGuiLightSlider(
		LightData& data,
		std::function<void()> resetButton
	) {
		if (ImGui::Begin("Light")) {
			ImGui::Text("Position");
			ImGui::SliderFloat("X", &data.source.x, -18.0f, 18.0f, "%.1f");
			ImGui::SliderFloat("Y", &data.source.y, -18.0f, 18.0f, "%.1f");
			ImGui::SliderFloat("Z", &data.source.z, -18.0f, 18.0f, "%.1f");

			ImGui::Text("Intensity/Color");
			ImGui::SliderFloat("Intensity", &data.diffuseIntensity, 0.01f, 2.0f, "%.2f");
			ImGui::ColorEdit3("Diffuse Color", &data.diffuseColor.x);
			ImGui::ColorEdit3("Ambient", &data.ambient.x);
			//ImGui::ColorEdit3("Material", &data.materialColor.x);

			ImGui::Text("Falloff");
			ImGui::SliderFloat("Constant", &data.attenuationConstant, 0.05f, 10.0f, "%.2f");
			ImGui::SliderFloat("Linear", &data.attenuationLinear, 0.0001f, 4.0f, "%.4f");
			ImGui::SliderFloat("Quadratic", &data.attenuationQuadratic, 0.0000001f, 10.0f, "%.7f");

			if (ImGui::Button("Reset"))
			{
				resetButton();
			}
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

	void ImGuiLightSlider(
		LightData& data,
		std::function<void()> resetButton
	) {}
}

#endif
