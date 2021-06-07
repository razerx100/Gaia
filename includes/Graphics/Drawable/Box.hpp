#ifndef __BOX_HPP__
#define __BOX_HPP__
#include <DrawableBase.hpp>
#include <TestObject.hpp>

class Box : public DrawableBase<Box> {
public:
	Box(Graphics& gfx,
		std::mt19937& rng,
		std::uniform_real_distribution<float>& adist,
		std::uniform_real_distribution<float>& ddist,
		std::uniform_real_distribution<float>& odist,
		std::uniform_real_distribution<float>& rdist,
		std::uniform_real_distribution<float>& bdist,
		DirectX::XMFLOAT4 material);

	void Update(float deltaTime) noexcept override;

private:
	TestObject m_TObj;
	DirectX::XMFLOAT3X3 m_Mat;
};

#endif
