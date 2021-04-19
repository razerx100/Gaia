#ifndef __LIGHT_HPP__
#define __LIGHT_HPP__
#include <DirectXMath.h>

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
	static DirectX::XMFLOAT3 GetLightPosition() noexcept;

	static void ImGuiLightSlider() noexcept;

private:
	struct pos {
		float x;
		float y;
		float z;
	};
	static pos LightPosition;
};
#endif
