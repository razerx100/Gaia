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
	float R;
	float Roll;
	float Pitch;
	float Yaw;
	float Theta;
	float Phi;
	float Chi;
	// speed (delta/s)
	float dRoll;
	float dPitch;
	float dYaw;
	float dTheta;
	float dPhi;
	float dChi;
};
#endif
