#ifndef __PLANE_HPP__
#define __PLANE_HPP__
#include <IndexedTriangleList.hpp>

class Plane {
public:
	static IndexedTriangleList MakeTesselated(
		std::uint16_t divisions_x, std::uint16_t divisions_y
	);

	static IndexedTriangleList Make();
};

#endif
