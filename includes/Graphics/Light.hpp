#ifndef __LIGHT_HPP__
#define __LIGHT_HPP__
#include <DirectXMath.h>
#include <memory>
#include <SolidSphere.hpp>

enum class LightType {
	Directional,
	Point
};

enum class ShadingType {
	PerVertex,
	PerPixel
};

struct LightData {
	DirectX::XMFLOAT3 source;
	float padding0;
	DirectX::XMFLOAT3 ambient;
	float padding1;
	DirectX::XMFLOAT3 diffuseColor;
	float diffuseIntensity;
	float attenuationConstant;
	float attenuationLinear;
	float attenuationQuadratic;
};

class Light {
public:
	Light(class Graphics& gfx, float radius);
	LightData GetLightData() const noexcept;

	void ImGuiLightSlider() noexcept;

	void Update() noexcept;
	void Draw(class Graphics& gfx) noexcept;

	void ResetData() noexcept;

private:
	LightData m_lightData;
	std::unique_ptr<class SolidSphere> m_lightSource;
};
#endif
