#ifndef __BOX_HPP__
#define __BOX_HPP__
#include <DrawableBase.hpp>

class Box : public DrawableBase<Box> {
public:
	Box(Graphics& gfx,
		DirectX::XMFLOAT4 material);

	void Draw(
		Graphics& gfx,
		const DirectX::XMMATRIX& accumulatedTransform
	) noexcept;

private:
	DirectX::XMFLOAT3X3 m_Mat;
};

#endif
