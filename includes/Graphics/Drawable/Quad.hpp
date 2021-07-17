#ifndef __QUAD_HPP__
#define __QUAD_HPP__
#include <Drawable.hpp>

class Quad : public Drawable {
public:
	Quad(
		Graphics& gfx,
		const std::string& objectName,
		const std::string& texturePath =  ""
	);

	void SetTransform(const DirectX::XMMATRIX& transform) noexcept;
};
#endif
