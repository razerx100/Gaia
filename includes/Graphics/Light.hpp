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

class Light {
public:
	Light(class Graphics& gfx, float radius);
	DirectX::XMFLOAT3 GetLightPosition() noexcept;

	void ImGuiLightSlider() noexcept;

	void Update() noexcept;
	void Draw(class Graphics& gfx) noexcept;

private:
	struct pos {
		float x;
		float y;
		float z;
	} m_lightPosition;
	std::unique_ptr<class SolidSphere> m_lightSource;
};
#endif
