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

IndexedTriangleList Cube::MakeSkinned() {

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
	vertices.emplace_back(-side, -side, -side);
	vertices.emplace_back(side, -side, -side);
	vertices.emplace_back(-side, -side, -side);
	vertices.emplace_back(-side, -side, side);
	vertices.emplace_back(side, -side, -side);
	vertices.emplace_back(side, -side, side);

	std::vector<std::uint16_t> indices = {
			0u, 2u, 1u,		2u, 3u, 1u,
			4u, 8u, 5u,		5u, 8u, 9u,
			2u, 6u, 3u,		3u, 6u, 7u,
			4u, 5u, 7u,		4u, 7u, 6u,
			2u, 10u, 11u,	2u, 11u, 6u,
			12u, 3u, 7u,	12u, 7u, 13u
	};

	return IndexedTriangleList(
		std::move(vertices),
		std::move(indices)
	);
}

IndexedTriangleList Cube::MakeIndependent() {
	constexpr float side = 1.0f / 2.0f;

	std::vector<DirectX::XMFLOAT3> vertices(24);
	vertices[0] = { -side,-side,-side };// 0 near side
	vertices[1] = { side,-side,-side };// 1
	vertices[2] = { -side,side,-side };// 2
	vertices[3] = { side,side,-side };// 3
	vertices[4] = { -side,-side,side };// 4 far side
	vertices[5] = { side,-side,side };// 5
	vertices[6] = { -side,side,side };// 6
	vertices[7] = { side,side,side };// 7
	vertices[8] = { -side,-side,-side };// 8 left side
	vertices[9] = { -side,side,-side };// 9
	vertices[10] = { -side,-side,side };// 10
	vertices[11] = { -side,side,side };// 11
	vertices[12] = { side,-side,-side };// 12 right side
	vertices[13] = { side,side,-side };// 13
	vertices[14] = { side,-side,side };// 14
	vertices[15] = { side,side,side };// 15
	vertices[16] = { -side,-side,-side };// 16 bottom side
	vertices[17] = { side,-side,-side };// 17
	vertices[18] = { -side,-side,side };// 18
	vertices[19] = { side,-side,side };// 19
	vertices[20] = { -side,side,-side };// 20 top side
	vertices[21] = { side,side,-side };// 21
	vertices[22] = { -side,side,side };// 22
	vertices[23] = { side,side,side };// 23

	return{
		std::move(vertices),{
			0u, 2u, 1u,			2u, 3u, 1u,
			4u, 5u, 7u,			4u, 7u, 6u,
			8u, 10u, 9u,		10u, 11u, 9u,
			12u, 13u, 15u,		12u, 15u, 14u,
			16u, 17u, 18u,		18u, 17u, 19u,
			20u, 23u, 21u,		20u, 22u, 23u
		}
	};
}
