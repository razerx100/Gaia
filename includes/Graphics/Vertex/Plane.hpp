#ifndef __PLANE_HPP__
#define __PLANE_HPP__
#include <IndexedTriangleList.hpp>

class Plane {
public:
	static IndexedTriangleList MakeTesselated(
		std::uint16_t divisions_x, std::uint16_t divisions_y
	);
	static IndexedTriangleList MakeTesselatedTextured(
		std::uint16_t divisions_x, std::uint16_t divisions_y
	);

	static IndexedTriangleList Make();

private:
	static std::vector<std::uint16_t> GenerateIndices(
		std::uint64_t indicesSize, int verticesNumberOnX,
		std::uint16_t divisions_x, std::uint16_t divisions_y
	) noexcept;
};

#endif
