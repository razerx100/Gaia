#ifndef __MUTABLE_HPP__
#define __MUTABLE_HPP__
#include <Bindable.hpp>


class Mutable : public Bindable {
public:
	virtual void Update(Graphics& gfx, void* data) = 0;
};
#endif
