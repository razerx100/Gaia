#include <Cube.hpp>

IndexedTriangleList Cube::Make() {

	constexpr float side = 1.0f / 2.0f;

	std::vector<DirectX::XMFLOAT3> vertices;
	vertices.emplace_back(-side, -side, -side);
	vertices.emplace_back(side, -side, -side);
	vertices.emplace_back(-side, side, -side);
	vertices.emplace_back(side, side, -side);
	vertices.emplace_back(-side, -side, side);
	vertices.emplace_back(side, -side, side);
	vertices.emplace_back(-side, side, side);
	vertices.emplace_back(side, side, side);

	std::vector<std::uint16_t> indices = {
			0u, 2u, 1u,		2u, 3u, 1u,
			1u, 3u, 5u,		3u, 7u, 5u,
			2u, 6u, 3u,		6u, 7u, 3u,
			4u, 5u, 7u,		4u, 7u, 6u,
			0u, 4u, 2u,		2u, 4u, 6u,
			0u, 1u, 4u,		1u, 5u, 4u
	};

	return IndexedTriangleList(
		std::move(vertices),
		std::move(indices)
	);
}
