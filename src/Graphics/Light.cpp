#include <Light.hpp>
#include <Graphics.hpp>
#include <ImGuiImpl.hpp>


Light::Light(Graphics& gfx, float radius) {

	ResetData();

	m_lightSource = std::make_unique<SolidSphere>(gfx, radius);
}

LightData Light::GetLightData() const noexcept {
	return m_lightData;
}

void Light::ImGuiLightSlider() noexcept {
	ImGuiImpl::ImGuiLightSlider(
		m_lightData,
		std::bind(&Light::ResetData, this)
	);
}

void Light::Update() noexcept {
	m_lightSource->SetPosition(
		m_lightData.source
	);
}

void Light::Draw(Graphics& gfx) noexcept {
	m_lightSource->Draw(gfx);
}

void Light::ResetData() noexcept {
	m_lightData.source = { 0.0f, 0.0f, -1.0f };
	m_lightData.ambient = { 0.05f, 0.05f, 0.05f };
	m_lightData.diffuseColor = { 1.0f, 1.0f, 1.0f };
	m_lightData.diffuseIntensity = 1.0f;
	m_lightData.attenuationConstant = 1.0f;
	m_lightData.attenuationLinear = 0.045f;
	m_lightData.attenuationQuadratic = 0.0075f;
}
