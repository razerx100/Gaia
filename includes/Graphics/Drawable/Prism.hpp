#ifndef __PRISM_HPP__
#define __PRISM_HPP__
#include <IndexedTriangleList.hpp>

class Prism {
public:
	static IndexedTriangleList MakeTesselated(std::uint16_t longDiv);
	static IndexedTriangleList Make();
};
#endif
