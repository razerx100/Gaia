#include <IndexedTriangleList.hpp>

IndexedTriangleList::IndexedTriangleList(
	std::vector<DirectX::XMFLOAT3>&& verticesInput,
	std::vector<std::uint16_t>&& indicesInput)
	: m_Vertices(std::move(verticesInput)), m_Indices(std::move(indicesInput)) {

	assert(m_Vertices.size() > 2);
	assert(m_Indices.size() % 3 == 0);
}

IndexedTriangleList::IndexedTriangleList(
	std::vector<DirectX::XMFLOAT3>&& verticesInput,
	std::vector<DirectX::XMFLOAT3>&& normalsInput,
	std::vector<std::uint16_t>&& indicesInput)
	:
	m_Vertices(std::move(verticesInput)),
	m_Normals(std::move(normalsInput)),
	m_Indices(std::move(indicesInput)) {

	assert(m_Vertices.size() > 2);
	assert(m_Indices.size() % 3 == 0);
}

void IndexedTriangleList::Transform(DirectX::FXMMATRIX matrix) {
	for (DirectX::XMFLOAT3& v : m_Vertices) {
		const DirectX::XMVECTOR position = DirectX::XMLoadFloat3(&v);

		DirectX::XMStoreFloat3(
			&v,
			DirectX::XMVector3Transform(position, matrix)
		);
	}
}

void IndexedTriangleList::SetNormalsIndependentFlat() noexcept {
	using namespace DirectX;

	m_Normals.resize(m_Indices.size());

	for (size_t i = 0; i < m_Indices.size(); i += 3) {
		XMFLOAT3& v0 = m_Vertices[m_Indices[i]];
		XMFLOAT3& v1 = m_Vertices[m_Indices[i + 1]];
		XMFLOAT3& v2 = m_Vertices[m_Indices[i + 2]];

		XMVECTOR p0 = XMLoadFloat3(&v0);
		XMVECTOR p1 = XMLoadFloat3(&v1);
		XMVECTOR p2 = XMLoadFloat3(&v2);

		XMVECTOR normal = XMVector3Normalize(
			XMVector3Cross((p1 - p0), (p2 - p0))
		);

		XMStoreFloat3(&m_Normals[m_Indices[i]], normal);
		XMStoreFloat3(&m_Normals[m_Indices[i + 1]], normal);
		XMStoreFloat3(&m_Normals[m_Indices[i + 2]], normal);
	}
}

std::vector<std::uint16_t> IndexedTriangleList::GetIndices() const noexcept {
	return m_Indices;
}

std::uint32_t IndexedTriangleList::GetVerticesSize() const noexcept {
	return static_cast<std::uint32_t>(m_Vertices.size());
}

Vertices IndexedTriangleList::GetVerticesObject(
	const VertexLayout& vertexLayout
) const noexcept {
	Vertices vertices = {
		vertexLayout,
		static_cast<std::uint32_t>(m_Vertices.size())
	};

	for (DirectX::XMFLOAT3 vertex : m_Vertices)
		vertices.AddVertexData(vertex);

	return vertices;
}

Vertices IndexedTriangleList::GetVerticesObject(
	const VertexLayout& vertexLayout,
	bool normals
) const noexcept {
	Vertices vertices = {
		vertexLayout,
		static_cast<std::uint32_t>(m_Vertices.size())
	};

	for (int i = 0; i < m_Vertices.size(); ++i)
		vertices.AddVertexData(m_Vertices[i], m_Normals[i]);

	return vertices;
}
