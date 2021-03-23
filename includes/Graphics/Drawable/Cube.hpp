#ifndef __CUBE_HPP__
#define __CUBE_HPP__
#include <Drawable.hpp>

class Cube : public Drawable {
public:
	Cube(Graphics& gfx, float posX, float posY);

	void Update(Graphics& gfx, float deltaTime) noexcept override;

private:
	float m_PosX;
	float m_PosY;
};
#endif
