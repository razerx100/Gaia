#include <TestObject.hpp>

TestObject::TestObject(
		std::mt19937& rng,
		std::uniform_real_distribution<float>& adist,
		std::uniform_real_distribution<float>& ddist,
		std::uniform_real_distribution<float>& odist,
		std::uniform_real_distribution<float>& rdist
		)
	:
	R(rdist(rng)),
	Roll(0.0f),
	Pitch(0.0f),
	Yaw(0.0f),
	Theta(adist(rng)),
	Phi(adist(rng)),
	Chi(adist(rng)),
	dRoll(ddist(rng)),
	dPitch(ddist(rng)),
	dYaw(ddist(rng)),
	dTheta(odist(rng)),
	dPhi(odist(rng)),
	dChi(odist(rng)) {}


DirectX::XMMATRIX TestObject::GetMomentum(float deltaTime) noexcept {
	Roll += dRoll * deltaTime;
	Pitch += dPitch * deltaTime;
	Yaw += dYaw * deltaTime;
	Theta += dTheta * deltaTime;
	Phi += dPhi * deltaTime;
	Chi += dChi * deltaTime;

	return
		DirectX::XMMatrixRotationRollPitchYaw(Pitch, Yaw, Roll) *
		DirectX::XMMatrixTranslation(R, 0.0f, 0.0f) *
		DirectX::XMMatrixRotationRollPitchYaw(Theta, Phi, Chi);
}
