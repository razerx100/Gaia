#ifndef __3DOBJECT_HPP__
#define __3DOBJECT_HPP__
#include <DrawableBase.hpp>
#include <random>

class D3Object : public DrawableBase<D3Object> {
public:
	D3Object(Graphics& gfx,
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
