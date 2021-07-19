#ifndef __LIGHT_HPP__
#define __LIGHT_HPP__
#include <DirectXMath.h>
#include <memory>

enum class LightType {
	Directional,
	Point
};

enum class ShadingType {
	PerVertex,
	PerPixel
};

struct LightData {
	DirectX::XMFLOAT3 lightPosition;
	float padding0;
	DirectX::XMFLOAT3 cameraPosition;
	float padding1;
	DirectX::XMFLOAT3 ambient;
	float padding2;
	DirectX::XMFLOAT3 diffuseColor;
	float diffuseIntensity;
	float attenuationConstant;
	float attenuationLinear;
	float attenuationQuadratic;
};

class Graphics;

class Light {
public:
	Light() = default;
	Light(Graphics& gfx, float radius);

	LightData GetLightData() const noexcept;

	static Light* GetLight() noexcept;
	static void UpdateLights(Graphics& gfx) noexcept;

	void Init(Graphics& gfx, float radius);
	void ImGuiLightSlider() noexcept;
	void Update() noexcept;
	void Draw(Graphics& gfx) noexcept;
	void ResetData() noexcept;

private:
	LightData m_lightData;
	std::unique_ptr<class SolidSphere> m_lightSource;

	static Light s_instance;
};
#endif
