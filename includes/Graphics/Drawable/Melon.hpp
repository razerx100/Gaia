#ifndef __MELON_HPP__
#define __MELON_HPP__
#include <DrawableBase.hpp>
#include <random>

class Melon : public DrawableBase<Melon> {
public:
	Melon(Graphics& gfx,
		std::mt19937& rng,
		std::uniform_real_distribution<float>& adist,
		std::uniform_real_distribution<float>& ddist,
		std::uniform_real_distribution<float>& odist,
		std::uniform_real_distribution<float>& rdist,
		std::uniform_int_distribution<int>& longdist,
		std::uniform_int_distribution<int>& latdist);

	void Update(float deltaTime) noexcept override;

	std::uint32_t GetIndexCount() const noexcept override;

private:
	// positional
	float r;
	float roll;
	float pitch;
	float yaw;
	float theta;
	float phi;
	float chi;
	// speed (delta/s)
	float droll;
	float dpitch;
	float dyaw;
	float dtheta;
	float dphi;
	float dchi;
};

#endif
