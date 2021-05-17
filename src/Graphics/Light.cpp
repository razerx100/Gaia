#include <Light.hpp>
#include <Graphics.hpp>
#include <ImGuiImpl.hpp>


Light::Light(Graphics& gfx, float radius)
	: m_lightPosition{0.0f, 0.0f, -1.0f} {

	m_lightSource = std::make_unique<SolidSphere>(gfx, radius);
}

DirectX::XMFLOAT3 Light::GetLightPosition() noexcept {
	return DirectX::XMFLOAT3(&m_lightPosition.x);
}

void Light::ImGuiLightSlider() noexcept {
	ImGuiImpl::ImGuiLightSlider(m_lightPosition.x, m_lightPosition.y, m_lightPosition.z);
}

void Light::Update() noexcept {
	m_lightSource->SetPosition(
		DirectX::XMFLOAT3(m_lightPosition.x, m_lightPosition.y, m_lightPosition.z)
	);
}

void Light::Draw(Graphics& gfx) noexcept {
	m_lightSource->Draw(gfx);
}
