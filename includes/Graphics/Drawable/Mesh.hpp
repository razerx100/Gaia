#ifndef __MESH_HPP__
#define __MESH_HPP__
#include <Drawable.hpp>
#include <memory>
#include <deque>

class Mesh : public Drawable {
public:
	Mesh(std::deque<std::unique_ptr<Bindable>>&& pBinds);

	void Draw(
		Graphics& gfx,
		const DirectX::XMMATRIX& accumulatedTransform
	) noexcept;

	Transform* GetTransform() noexcept;
};
#endif
