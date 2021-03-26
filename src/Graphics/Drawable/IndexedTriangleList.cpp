#include <IndexedTriangleList.hpp>

IndexedTriangleList::IndexedTriangleList(
	std::vector<DirectX::XMFLOAT3>& vertices_input,
	std::vector<std::uint16_t>& indices_input)
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
