#include <IndexedTriangleList.hpp>

void IndexedTriangleList::SetVertices(
	std::vector<DirectX::XMFLOAT3>&& verticesInput) noexcept {
	m_vertices = std::move(verticesInput);
}

void IndexedTriangleList::SetIndices(
	std::vector<std::uint16_t>&& indicesInput) noexcept {
	m_indices = std::move(indicesInput);
}

void IndexedTriangleList::SetNormals(
	std::vector<DirectX::XMFLOAT3>&& normalsInput) noexcept {
	m_normals = std::move(normalsInput);
}

void IndexedTriangleList::SetUVs(
	std::vector<DirectX::XMFLOAT2>&& uvsInput) noexcept {
	m_uvs = std::move(uvsInput);
}

void IndexedTriangleList::Transform(const DirectX::XMMATRIX& matrix) {
	for (DirectX::XMFLOAT3& v : m_vertices) {
		const DirectX::XMVECTOR position = DirectX::XMLoadFloat3(&v);

		DirectX::XMStoreFloat3(
			&v,
			DirectX::XMVector3Transform(position, matrix)
		);
	}
}

void IndexedTriangleList::SetNormalsIndependentFlat() noexcept {
	using namespace DirectX;

	m_normals.resize(m_indices.size());

	for (size_t i = 0; i < m_indices.size(); i += 3) {
		XMFLOAT3& v0 = m_vertices[m_indices[i]];
		XMFLOAT3& v1 = m_vertices[m_indices[i + 1]];
		XMFLOAT3& v2 = m_vertices[m_indices[i + 2]];

		XMVECTOR p0 = XMLoadFloat3(&v0);
		XMVECTOR p1 = XMLoadFloat3(&v1);
		XMVECTOR p2 = XMLoadFloat3(&v2);

		XMVECTOR normal = XMVector3Normalize(
			XMVector3Cross((p1 - p0), (p2 - p0))
		);

		XMStoreFloat3(&m_normals[m_indices[i]], normal);
		XMStoreFloat3(&m_normals[m_indices[i + 1]], normal);
		XMStoreFloat3(&m_normals[m_indices[i + 2]], normal);
	}
}

std::vector<std::uint16_t> IndexedTriangleList::GetIndices() const noexcept {
	return m_indices;
}

std::uint32_t IndexedTriangleList::GetVerticesSize() const noexcept {
	return static_cast<std::uint32_t>(m_vertices.size());
}

Vertices IndexedTriangleList::GetVerticesObject(
	const VertexLayout& vertexLayout,
	LegacyType type
) const noexcept {
	Vertices vertices;
	if (type == LegacyType::SolidColorNoNorm) {
		vertices.Init(
			vertexLayout,
			static_cast<std::uint32_t>(m_vertices.size())
		);

		for (DirectX::XMFLOAT3 vertex : m_vertices)
			vertices.AddVertexData(vertex);
	}
	else if (type == LegacyType::SolidColor) {
		vertices.Init(
			vertexLayout,
			static_cast<std::uint32_t>(m_vertices.size())
		);

		for (int i = 0; i < m_vertices.size(); ++i)
			vertices.AddVertexData(m_vertices[i], m_normals[i]);
	}
	else if (type == LegacyType::WithTexture) {
		vertices.Init(
			vertexLayout,
			static_cast<std::uint32_t>(m_vertices.size())
		);

		for (int i = 0; i < m_vertices.size(); ++i)
			vertices.AddVertexData(m_vertices[i], m_normals[i], m_uvs[i]);
	}

	return vertices;
}
