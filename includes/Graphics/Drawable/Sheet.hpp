#ifndef __SHEET_HPP__
#define __SHEET_HPP__
#include <DrawableBase.hpp>
#include <random>

class Sheet : public DrawableBase<Sheet> {
public:
	Sheet(Graphics& gfx,
		std::mt19937& rng,
		std::uniform_real_distribution<float>& adist,
		std::uniform_real_distribution<float>& ddist,
		std::uniform_real_distribution<float>& odist,
		std::uniform_real_distribution<float>& rdist);

	void Update(float deltaTime) noexcept override;

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

	static ConstantBuffer<DirectX::XMMATRIX>* s_pVCBuffer;
};
#endif
