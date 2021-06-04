#include <VertexBuffer.hpp>
#include <Graphics.hpp>

VertexBuffer::VertexBuffer(std::vector<DirectX::XMFLOAT3>&& vertices)
	: m_VertexBufferView{} {

	const std::uint32_t stride = static_cast<std::uint32_t>(sizeof(DirectX::XMFLOAT3));
	const std::uint32_t verticesSize = static_cast<std::uint32_t>(
			std::size(vertices) * stride
		);

	m_pBuffer = BufferMan::RequestMemory(verticesSize);
	memcpy(m_pBuffer->cpuPTR, vertices.data(), verticesSize);

	m_VertexBufferView.BufferLocation = m_pBuffer->gpuPTR;
	m_VertexBufferView.SizeInBytes = verticesSize;
	m_VertexBufferView.StrideInBytes = stride;
}

VertexBuffer::VertexBuffer(const Vertices& vertices)
	: m_VertexBufferView{} {

	m_pBuffer = BufferMan::RequestMemory(vertices.GetSizeByte());
	memcpy(m_pBuffer->cpuPTR, vertices.GetBuffer(), vertices.GetSizeByte());

	m_VertexBufferView.BufferLocation = m_pBuffer->gpuPTR;
	m_VertexBufferView.SizeInBytes = vertices.GetSizeByte();
	m_VertexBufferView.StrideInBytes = vertices.GetVertexSizeByte();
}

VertexBuffer::VertexBuffer(
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

	m_pBuffer = BufferMan::RequestMemory(verticesSize);
	memcpy(m_pBuffer->cpuPTR, vVertices.data(), verticesSize);

	m_VertexBufferView.BufferLocation = m_pBuffer->gpuPTR;
	m_VertexBufferView.SizeInBytes = verticesSize;
	m_VertexBufferView.StrideInBytes = stride;
}

VertexBuffer::VertexBuffer(
	std::vector<DirectX::XMFLOAT3>&& vertices,
	std::vector<DirectX::XMFLOAT3>&& normals
) : m_VertexBufferView{} {

	struct Vertex {
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
	};

	const std::uint32_t stride = static_cast<std::uint32_t>(sizeof(Vertex));

	std::vector<Vertex> vVertices;

	for (int i = 0; i < vertices.size(); i++)
		vVertices.emplace_back(Vertex(std::move(vertices[i]), std::move(normals[i])));

	const std::uint32_t verticesSize = static_cast<std::uint32_t>(
			std::size(vVertices) * stride
		);

	m_pBuffer = BufferMan::RequestMemory(verticesSize);
	memcpy(m_pBuffer->cpuPTR, vVertices.data(), verticesSize);

	m_VertexBufferView.BufferLocation = m_pBuffer->gpuPTR;
	m_VertexBufferView.SizeInBytes = verticesSize;
	m_VertexBufferView.StrideInBytes = stride;
}

VertexBuffer::VertexBuffer(
	std::vector<DirectX::XMFLOAT3>&& vertices,
	std::vector<DirectX::XMFLOAT3>&& normals,
	std::vector<DirectX::XMFLOAT4>&& colors
) : m_VertexBufferView{} {

	struct Vertex {
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT4 colors;
	};

	const std::uint32_t stride = static_cast<std::uint32_t>(sizeof(Vertex));

	std::vector<Vertex> vVertices;

	for (int i = 0; i < vertices.size(); i++)
		vVertices.emplace_back(Vertex(std::move(vertices[i]),
			std::move(normals[i]), std::move(colors[i])));

	const std::uint32_t verticesSize = static_cast<std::uint32_t>(
			std::size(vVertices) * stride
		);

	m_pBuffer = BufferMan::RequestMemory(verticesSize);
	memcpy(m_pBuffer->cpuPTR, vVertices.data(), verticesSize);

	m_VertexBufferView.BufferLocation = m_pBuffer->gpuPTR;
	m_VertexBufferView.SizeInBytes = verticesSize;
	m_VertexBufferView.StrideInBytes = stride;
}

VertexBuffer::VertexBuffer(
	std::vector<DirectX::XMFLOAT3>&& vertices,
	std::vector<DirectX::XMFLOAT3>&& normals,
	std::vector<DirectX::XMFLOAT2>&& uvs
) : m_VertexBufferView{} {

	struct Vertex {
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normals;
		DirectX::XMFLOAT2 coordinates;
	};

	const std::uint32_t stride = static_cast<std::uint32_t>(sizeof(Vertex));

	std::vector<Vertex> vVertices;

	for (int i = 0; i < vertices.size(); i++)
		vVertices.emplace_back(Vertex(std::move(vertices[i]),
			std::move(normals[i]), std::move(uvs[i])));

	const std::uint32_t verticesSize = static_cast<std::uint32_t>(
			std::size(vVertices) * stride
		);

	m_pBuffer = BufferMan::RequestMemory(verticesSize);
	memcpy(m_pBuffer->cpuPTR, vVertices.data(), verticesSize);

	m_VertexBufferView.BufferLocation = m_pBuffer->gpuPTR;
	m_VertexBufferView.SizeInBytes = verticesSize;
	m_VertexBufferView.StrideInBytes = stride;
}

VertexBuffer::VertexBuffer(
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

	m_pBuffer = BufferMan::RequestMemory(verticesSize);
	memcpy(m_pBuffer->cpuPTR, vVertices.data(), verticesSize);

	m_VertexBufferView.BufferLocation = m_pBuffer->gpuPTR;
	m_VertexBufferView.SizeInBytes = verticesSize;
	m_VertexBufferView.StrideInBytes = stride;
}

void VertexBuffer::BindCommand(Graphics& gfx) noexcept {
	GetCommandList(gfx)->IASetVertexBuffers(0u, 1u, &m_VertexBufferView);
}
