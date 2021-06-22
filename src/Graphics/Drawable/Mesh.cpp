#include <Mesh.hpp>
#include <Bindable.hpp>
#include <IndexBuffer.hpp>

Mesh::Mesh(
	std::deque<std::unique_ptr<Bindable>>&& pBinds,
	std::unique_ptr<IndexBuffer> pIndexBuffer
) {
	AddIndexBuffer(std::move(pIndexBuffer));

	for (auto& pBind : pBinds)
		AddBind(std::move(pBind));
}

void Mesh::Draw(
	Graphics& gfx,
	const DirectX::XMMATRIX& accumulatedTransform
) noexcept {
	m_Transform = accumulatedTransform;

	Drawable::Draw(gfx);
}

Transform* Mesh::GetTransform() noexcept {
	return &m_Transform;
}
