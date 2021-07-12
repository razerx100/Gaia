#include <Mesh.hpp>
#include <Bindable.hpp>
#include <IndexBuffer.hpp>
#include <BindableCodex.hpp>

Mesh::Mesh(std::vector<BindPtr*>&& pBindRefs) {
	for (auto& pBind : pBindRefs)
		AddBind(std::move(pBind));
}

Mesh::Mesh(
	std::vector<BindPtr*>&& pBindRefs,
	std::vector<std::unique_ptr<Bindable>>&& pBinds
) {
	for (auto& pBind : pBindRefs)
		AddBind(std::move(pBind));

	AddBinds(std::move(pBinds));
}

void Mesh::AddBinds(std::vector<std::unique_ptr<Bindable>>&& pBinds) noexcept {
	for (auto& pBind : pBinds)
		AddBind(std::move(pBind));
}

void Mesh::Draw(
	Graphics& gfx,
	const DirectX::XMMATRIX& accumulatedTransform
) noexcept {
	m_transform = accumulatedTransform;

	Drawable::Draw(gfx);
}

Transform* Mesh::GetTransform() noexcept {
	return &m_transform;
}
