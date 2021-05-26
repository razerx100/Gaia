#ifndef __CYLINDER_HPP__
#define __CYLINDER_HPP__
#include <DrawableBase.hpp>
#include <TestObject.hpp>

class Cylinder : public DrawableBase<Cylinder> {
public:
	Cylinder(
		Graphics& gfx,
		std::mt19937& rng,
		std::uniform_real_distribution<float>& adist,
		std::uniform_real_distribution<float>& ddist,
		std::uniform_real_distribution<float>& odist,
		std::uniform_real_distribution<float>& rdist,
		std::uniform_int_distribution<int>& tdist,
		DirectX::XMFLOAT4 material);

	void Update(float deltaTime) noexcept override;

private:
	TestObject m_tobj;
};
#endif
