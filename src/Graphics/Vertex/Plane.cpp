#include <Plane.hpp>
#include <array>

IndexedTriangleList Plane::MakeTesselated(std::uint16_t divisions_x, std::uint16_t divisions_y) {
	assert(divisions_x > 0);
	assert(divisions_y > 0);

	constexpr float width = 2.0f;
	constexpr float height = 2.0f;
	const int nVertices_x = divisions_x + 1;
	const int nVertices_y = divisions_y + 1;

	// Vertices
	size_t verticesNumber = nVertices_x * nVertices_y;
	std::vector<DirectX::XMFLOAT3> vertices(verticesNumber);
	std::vector<DirectX::XMFLOAT3> normals(
		verticesNumber, DirectX::XMFLOAT3{0.0f, 0.0f, -1.0f}
	);

	const float side_x = width / 2.0f;
	const float side_y = height / 2.0f;

	const float divisionSize_x = width / divisions_x;
	const float divisionSize_y = height / divisions_y;

	const DirectX::XMVECTOR bottomLeft = DirectX::XMVectorSet(-side_x, -side_y, 0.0f, 0.0f);

	for (int y = 0, i = 0; y < nVertices_y; y++) {
		const float y_pos = y * divisionSize_y;
		for (int x = 0; x < nVertices_x; x++, i++) {
			const DirectX::XMVECTOR v = DirectX::XMVectorAdd(
				bottomLeft,
				DirectX::XMVectorSet(x * divisionSize_x, y_pos, 0.0f, 0.0f)
			);

			DirectX::XMStoreFloat3(&vertices[i], v);
		}
	}

	// Indices
	std::vector<std::uint16_t> indices = GenerateIndices(
		static_cast<std::uint64_t>(pow(divisionSize_x * divisionSize_y, 2) * 6),
		nVertices_x, divisions_x, divisions_y
	);

	IndexedTriangleList data;
	data.SetVertices(std::move(vertices));
	data.SetIndices(std::move(indices));
	data.SetNormals(std::move(normals));

	return data;
}

IndexedTriangleList Plane::MakeTesselatedTextured(
	std::uint16_t divisions_x, std::uint16_t divisions_y
) {
	assert(divisions_x > 0);
	assert(divisions_y > 0);

	constexpr float width = 2.0f;
	constexpr float height = 2.0f;
	const int nVertices_x = divisions_x + 1;
	const int nVertices_y = divisions_y + 1;

	// Vertices
	size_t verticesNumber = nVertices_x * nVertices_y;
	std::vector<DirectX::XMFLOAT3> vertices(verticesNumber);
	std::vector<DirectX::XMFLOAT3> normals(
		verticesNumber, DirectX::XMFLOAT3{0.0f, 0.0f, -1.0f}
	);
	std::vector<DirectX::XMFLOAT2> uvs(verticesNumber);

	const float side_x = width / 2.0f;
	const float side_y = height / 2.0f;

	const float divisionSize_x = width / divisions_x;
	const float divisionSize_y = height / divisions_y;
	const float divisionSize_x_tc = 1.0f / divisions_x;
	const float divisionSize_y_tc = 1.0f / divisions_y;

	const DirectX::XMVECTOR bottomLeft = DirectX::XMVectorSet(-side_x, -side_y, 0.0f, 0.0f);

	for (int y = 0, i = 0; y < nVertices_y; y++) {
		const float y_pos = y * divisionSize_y;
		const float y_pos_tc = 1.0f - y * divisionSize_y_tc;
		for (int x = 0; x < nVertices_x; x++, i++) {
			const DirectX::XMVECTOR v = DirectX::XMVectorAdd(
				bottomLeft,
				DirectX::XMVectorSet(x * divisionSize_x, y_pos, 0.0f, 0.0f)
			);

			const float x_pos_tc = static_cast<float>(x) * divisionSize_x_tc;

			DirectX::XMStoreFloat3(&vertices[i], v);
			uvs[i] = DirectX::XMFLOAT2{ x_pos_tc, y_pos_tc };
		}
	}

	// Indices
	std::vector<std::uint16_t> indices = GenerateIndices(
		static_cast<std::uint64_t>(pow(divisionSize_x * divisionSize_y, 2) * 6),
		nVertices_x, divisions_x, divisions_y
	);

	IndexedTriangleList data;
	data.SetVertices(std::move(vertices));
	data.SetIndices(std::move(indices));
	data.SetNormals(std::move(normals));
	data.SetUVs(std::move(uvs));

	return data;
}

IndexedTriangleList Plane::Make() {
	return MakeTesselated(2, 2);
}

std::vector<std::uint16_t> Plane::GenerateIndices(
	std::uint64_t indicesSize, int verticesNumberOnX,
	std::uint16_t divisions_x, std::uint16_t divisions_y
) noexcept {
	std::vector<std::uint16_t> indices;
	indices.reserve(indicesSize);

	const auto vxy2i = [verticesNumberOnX]
	(std::uint16_t x, std::uint16_t y)->std::uint16_t {
		return static_cast<std::uint16_t>(y * verticesNumberOnX + x);
	};

	for (std::uint16_t y = 0; y < divisions_y; y++) {
		for (std::uint16_t x = 0; x < divisions_x; x++) {
			const std::array<std::uint16_t, 4> indexArray = {
				vxy2i(x, y), vxy2i(x + 1, y), vxy2i(x, y + 1), vxy2i(x + 1, y + 1)
			};

			indices.push_back(indexArray[0]);
			indices.push_back(indexArray[2]);
			indices.push_back(indexArray[1]);
			indices.push_back(indexArray[1]);
			indices.push_back(indexArray[2]);
			indices.push_back(indexArray[3]);
		}
	}

	return indices;
}
