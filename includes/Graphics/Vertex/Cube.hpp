#ifndef __CUBE_HPP__
#define __CUBE_HPP__
#include <IndexedTriangleList.hpp>

class Cube {
public:
	static IndexedTriangleList Make();
	static IndexedTriangleList MakeSkinned();
	static IndexedTriangleList MakeIndependent();
};

#endif
