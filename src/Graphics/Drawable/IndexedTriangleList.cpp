#include <IndexedTriangleList.hpp>

IndexedTriangleList::IndexedTriangleList(
	std::vector<DirectX::XMFLOAT3>&& vertices_input,
	std::vector<std::uint16_t>&& indices_input)
	: m_Vertices(std::move(vertices_input)), m_Indices(std::move(indices_input)) {

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
