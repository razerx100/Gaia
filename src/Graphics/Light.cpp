#include <Light.hpp>
#include <ImGuiImpl.hpp>

Light::pos Light::LightPosition{ 0.0f, 0.0f, -1.0f };

DirectX::XMFLOAT3 Light::GetLightPosition() noexcept {
	return DirectX::XMFLOAT3(&LightPosition.x);
}

void Light::ImGuiLightSlider() noexcept {
	ImGuiImpl::ImGuiLightSlider(LightPosition.x, LightPosition.y, LightPosition.z);
}
