#ifndef __TOPOLOGY_HPP__
#define __TOPOLOGY_HPP__
#include <Bindable.hpp>

class Topology : public Bindable {
public:
	Topology(D3D_PRIMITIVE_TOPOLOGY topology);

	void Bind(Graphics& gfx) noexcept override;

private:
	D3D_PRIMITIVE_TOPOLOGY m_Topology;
};
#endif
