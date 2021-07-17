#ifndef __INDEX_TRIANGLE_LIST_HPP__
#define __INDEX_TRIANGLE_LIST_HPP__
#include <vector>
#include <DirectXMath.h>
#include <Vertices.hpp>
#include <optional>
#include <functional>

enum class LegacyType {
	SolidColor,
	SolidColorNoNorm,
	WithTexture
};

class IndexedTriangleList {
public:
	IndexedTriangleList() = default;
	IndexedTriangleList(
		std::vector<DirectX::XMFLOAT3>&& verticesInput,
		std::vector<std::uint16_t>&& indicesInput);
	IndexedTriangleList(
		std::vector<DirectX::XMFLOAT3>&& verticesInput,
		std::vector<DirectX::XMFLOAT3>&& normalsInput,
		std::vector<std::uint16_t>&& indicesInput);
	IndexedTriangleList(
		std::vector<DirectX::XMFLOAT3>&& verticesInput,
		std::vector<DirectX::XMFLOAT3>&& normalsInput,
		std::vector<DirectX::XMFLOAT2>&& uvsInput,
		std::vector<std::uint16_t>&& indicesInput);

	void Transform(const DirectX::XMMATRIX& matrix);
	void SetNormalsIndependentFlat() noexcept;

	std::uint32_t GetVerticesSize() const noexcept;

public:
	std::vector<std::uint16_t> GetIndices() const noexcept;

	// Positions only
	Vertices GetVerticesObject(
		const VertexLayout& vertexLayout,
		LegacyType type
	) const noexcept;

private:
	std::vector<DirectX::XMFLOAT3> m_vertices;
	std::vector<std::uint16_t> m_indices;
	std::vector<DirectX::XMFLOAT3> m_normals;
	std::vector<DirectX::XMFLOAT2> m_uvs;
};

#endif
