#ifndef __SKINNED_BOX_HPP__
#define __SKINNED_BOX_HPP__
#include <DrawableBase.hpp>
#include <TestObject.hpp>

class Tex2DBox : public DrawableBase<Tex2DBox> {
public:
	Tex2DBox(Graphics& gfx,
		std::mt19937& rng,
		std::uniform_real_distribution<float>& adist,
		std::uniform_real_distribution<float>& ddist,
		std::uniform_real_distribution<float>& odist,
		std::uniform_real_distribution<float>& rdist);

	void Update(float deltaTime) noexcept override;

private:
	TestObject m_TObj;
};

#endif
