#ifndef __RANDOM_OBJECT_HPP__
#define __RANDOM_OBJECT_HPP__
#include <random>
#include <DirectXMath.h>

class TestObject {
public:
	TestObject(
		std::mt19937& rng,
		std::uniform_real_distribution<float>& adist,
		std::uniform_real_distribution<float>& ddist,
		std::uniform_real_distribution<float>& odist,
		std::uniform_real_distribution<float>& rdist
	);

	DirectX::XMMATRIX GetMomentum(float deltaTime) noexcept;

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
