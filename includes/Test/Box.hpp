#ifndef __BOX_HPP__
#define __BOX_HPP__
#include <Drawable.hpp>

class Box : public Drawable {
public:
	Box(Graphics& gfx,
		const std::string& name,
		const std::string& texPath);

	void SetTransform(
		const DirectX::XMMATRIX& accumulatedTransform
	) noexcept;
};

#endif
