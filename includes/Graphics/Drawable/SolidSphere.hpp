#ifndef __SOLID_SPHERE_HPP__
#define __SOLID_SPHERE_HPP__
#include <DrawableBase.hpp>

class SolidSphere : public DrawableBase<SolidSphere> {
public:
	SolidSphere(Graphics& gfx, float radius);
	void SetPosition(DirectX::XMFLOAT3 position) noexcept;
};
#endif
