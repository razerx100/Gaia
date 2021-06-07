#ifndef __PYRAMID_HPP__
#define __PYRAMID_HPP__
#include <TestObject.hpp>
#include <DrawableBase.hpp>

class Pyramid : public DrawableBase<Pyramid> {
public:
	Pyramid(Graphics& gfx,
		std::mt19937& rng,
		std::uniform_real_distribution<float>& adist,
		std::uniform_real_distribution<float>& ddist,
		std::uniform_real_distribution<float>& odist,
		std::uniform_real_distribution<float>& rdist,
		std::uniform_int_distribution<int>& tdist);

	void Update(float deltaTime) noexcept override;

	std::uint32_t GetIndexCount() const noexcept override;

private:
	TestObject m_TObj;
};

#endif
