#ifndef __PRISM_HPP__
#define __PRISM_HPP__
#include <IndexedTriangleList.hpp>

class Prism {
public:
	static IndexedTriangleList MakeTesselated(int longDiv);
	static IndexedTriangleList MakeTesselatedIndependentCapNormals(int longDiv);

	static IndexedTriangleList Make();
};
#endif
