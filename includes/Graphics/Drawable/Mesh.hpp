#ifndef __MESH_HPP__
#define __MESH_HPP__
#include <Drawable.hpp>
#include <memory>
#include <vector>

class Mesh : public Drawable {
public:
	Mesh(std::vector<BindPtr*>&& pBindRefs, const std::string& name);
	Mesh(
		std::vector<BindPtr*>&& pBindRefs,
		std::vector<std::unique_ptr<Bindable>>&& pBinds,
		const std::string& name
	);

	void AddBinds(std::vector<std::unique_ptr<Bindable>>&& pBinds) noexcept;
	void Draw(
		Graphics& gfx,
		const DirectX::XMMATRIX& accumulatedTransform
	) noexcept;

	Transform* GetTransform() noexcept;
};
#endif
