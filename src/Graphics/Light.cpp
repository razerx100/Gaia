#include <Light.hpp>
#include <Graphics.hpp>
#include <ImGuiImpl.hpp>
#include <SolidSphere.hpp>
#include <Camera.hpp>

Light Light::s_instance;

Light::Light(Graphics& gfx, float radius) {
	Init(gfx, radius);
}

void Light::Init(Graphics& gfx, float radius) {
	ResetData();

	m_lightSource = std::make_unique<SolidSphere>(gfx, radius, "LightSource");
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
		m_lightData.lightPosition
	);
	m_lightData.cameraPosition = Camera::GetCameraPos();
}

void Light::UpdateLights(Graphics& gfx) noexcept {
	s_instance.Update();
	s_instance.Draw(gfx);
	s_instance.ImGuiLightSlider();
}

void Light::Draw(Graphics& gfx) noexcept {
	m_lightSource->Draw(gfx);
}

void Light::ResetData() noexcept {
	m_lightData.lightPosition = { 1.0f, 3.5f, -10.5f };
	m_lightData.ambient = { 0.05f, 0.05f, 0.05f };
	m_lightData.diffuseColor = { 1.0f, 1.0f, 1.0f };
	m_lightData.diffuseIntensity = 1.0f;
	m_lightData.attenuationConstant = 1.0f;
	m_lightData.attenuationLinear = 0.045f;
	m_lightData.attenuationQuadratic = 0.0075f;
}

Light* Light::GetLight() noexcept {
	return &s_instance;
}
