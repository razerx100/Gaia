#ifndef __SPHERE_HPP__
#define	__SPHERE_HPP__
#include <IndexedTriangleList.hpp>

class Sphere {
public:
	static IndexedTriangleList MakeTesselated(std::uint16_t latDiv, std::uint16_t longDiv);
	static IndexedTriangleList Make();
};

#endif
