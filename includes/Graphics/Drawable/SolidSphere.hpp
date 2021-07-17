#ifndef __SOLID_SPHERE_HPP__
#define __SOLID_SPHERE_HPP__
#include <Drawable.hpp>

class SolidSphere : public Drawable {
public:
	SolidSphere(Graphics& gfx, float radius, const std::string& name);

	void SetPosition(const DirectX::XMFLOAT3& position) noexcept;
};
#endif
