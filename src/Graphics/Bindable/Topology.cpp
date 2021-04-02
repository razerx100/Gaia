#include <Topology.hpp>

Topology::Topology(D3D12_PRIMITIVE_TOPOLOGY_TYPE PSOTopology, D3D_PRIMITIVE_TOPOLOGY IATopology)
	: m_IATopology(IATopology), m_PSOTopology(PSOTopology) {}

void Topology::BindCommand(Graphics& gfx) noexcept {
	GetCommandList(gfx)->IASetPrimitiveTopology(m_IATopology);
}


D3D12_PRIMITIVE_TOPOLOGY_TYPE Topology::GetTopology() const noexcept {
	return m_PSOTopology;
}
