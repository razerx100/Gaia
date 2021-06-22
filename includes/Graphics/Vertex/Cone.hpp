#ifndef __CONE_HPP__
#define __CONE_HPP__
#include <IndexedTriangleList.hpp>

class Cone {
public:
	static IndexedTriangleList MakeTesselated(std::uint16_t longDiv);

	static IndexedTriangleList MakeTesselatedIndependentFaces(int longDiv);

	static IndexedTriangleList Make();
};
#endif
