#include <Topology.hpp>

Topology::Topology(D3D_PRIMITIVE_TOPOLOGY topology)
	: m_Topology(topology) {}

void Topology::Bind(Graphics& gfx) noexcept {
	GetDeviceContext(gfx)->IASetPrimitiveTopology(m_Topology);
}
