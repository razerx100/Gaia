#ifndef __MODEL_HPP__
#define __MODEL_HPP__
#include <DrawableBase.hpp>
#include <TestObject.hpp>

class Model : public DrawableBase<Model> {
public:
	Model(Graphics& gfx,
		std::mt19937& rng,
		std::uniform_real_distribution<float>& adist,
		std::uniform_real_distribution<float>& ddist,
		std::uniform_real_distribution<float>& odist,
		std::uniform_real_distribution<float>& rdist,
		DirectX::XMFLOAT4 material, float scale);

	void Update(float deltaTime) noexcept override;

private:
	TestObject m_tobj;
};

#endif
