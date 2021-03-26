#ifndef __INDEX_TRIANGLE_LIST_HPP__
#define __INDEX_TRIANGLE_LIST_HPP__
#include <vector>
#include <DirectXMath.h>

class IndexedTriangleList {
public:
	IndexedTriangleList() = default;
	IndexedTriangleList(
		std::vector<DirectX::XMFLOAT3>& vertices_input,
		std::vector<std::uint16_t>& indices_input);

	void Transform(DirectX::FXMMATRIX matrix);

public:
	std::vector<DirectX::XMFLOAT3> m_Vertices;
	std::vector<std::uint16_t> m_Indices;
};

#endif
