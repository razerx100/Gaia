#include <VertexBuffer.hpp>

VertexBuffer::VertexBuffer(Graphics& gfx, std::vector<DirectX::XMFLOAT3>&& vertices)
	: m_VertexBufferView{} {
	const std::uint32_t stride = static_cast<std::uint32_t>(sizeof(DirectX::XMFLOAT3));
	const std::uint32_t verticesSize = static_cast<std::uint32_t>(
			std::size(vertices) * stride
		);

	CreateResource(gfx, vertices.data(), verticesSize, &m_pVertexBuffer);

	m_VertexBufferView.BufferLocation = m_pVertexBuffer->GetGPUVirtualAddress();
	m_VertexBufferView.SizeInBytes = verticesSize;
	m_VertexBufferView.StrideInBytes = stride;
}

void VertexBuffer::BindCommand(Graphics& gfx) noexcept {
	GetCommandList(gfx)->IASetVertexBuffers(0u, 1u, &m_VertexBufferView);
}
