#include <VertexBuffer.hpp>
#include <Graphics.hpp>

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

VertexBuffer::VertexBuffer(Graphics& gfx,
	std::vector<DirectX::XMFLOAT3>&& vertices,
	std::vector<DirectX::XMFLOAT4>&& colors
) : m_VertexBufferView{} {

	struct Vertex {
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
	};

	const std::uint32_t stride = static_cast<std::uint32_t>(sizeof(Vertex));

	std::vector<Vertex> vVertices;

	for (int i = 0; i < vertices.size(); i++)
		vVertices.emplace_back(Vertex(std::move(vertices[i]), std::move(colors[i])));

	const std::uint32_t verticesSize = static_cast<std::uint32_t>(
			std::size(vVertices) * stride
		);

	CreateResource(gfx, vVertices.data(), verticesSize, &m_pVertexBuffer);

	m_VertexBufferView.BufferLocation = m_pVertexBuffer->GetGPUVirtualAddress();
	m_VertexBufferView.SizeInBytes = verticesSize;
	m_VertexBufferView.StrideInBytes = stride;
}

VertexBuffer::VertexBuffer(Graphics& gfx,
	std::vector<DirectX::XMFLOAT3>&& vertices,
	std::vector<DirectX::XMFLOAT2>&& uvs
) : m_VertexBufferView{} {

	struct Vertex {
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 coordinates;
	};

	const std::uint32_t stride = static_cast<std::uint32_t>(sizeof(Vertex));

	std::vector<Vertex> vVertices;

	for (int i = 0; i < vertices.size(); i++)
		vVertices.emplace_back(Vertex(std::move(vertices[i]), std::move(uvs[i])));

	const std::uint32_t verticesSize = static_cast<std::uint32_t>(
			std::size(vVertices) * stride
		);

	CreateResource(gfx, vVertices.data(), verticesSize, &m_pVertexBuffer);

	m_VertexBufferView.BufferLocation = m_pVertexBuffer->GetGPUVirtualAddress();
	m_VertexBufferView.SizeInBytes = verticesSize;
	m_VertexBufferView.StrideInBytes = stride;
}

void VertexBuffer::BindCommand(Graphics& gfx) noexcept {
	GetCommandList(gfx)->IASetVertexBuffers(0u, 1u, &m_VertexBufferView);
}
