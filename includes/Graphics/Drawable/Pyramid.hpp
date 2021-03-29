#ifndef __PYRAMID_HPP__
#define __PYRAMID_HPP__
#include <DrawableBase.hpp>
#include <random>

class Pyramid : public DrawableBase<Pyramid> {
public:
	Pyramid(Graphics& gfx,
		std::mt19937& rng,
		std::uniform_real_distribution<float>& adist,
		std::uniform_real_distribution<float>& ddist,
		std::uniform_real_distribution<float>& odist,
		std::uniform_real_distribution<float>& rdist);

	void Update(Graphics& gfx, float deltaTime) noexcept override;

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
