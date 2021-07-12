#include <Topology.hpp>
#include <Graphics.hpp>
#include <unordered_map>
#include <Exception.hpp>

const std::unordered_map<std::string,
	std::pair<D3D_PRIMITIVE_TOPOLOGY, D3D12_PRIMITIVE_TOPOLOGY_TYPE>>
	topologyMap = {
		{"PointList",
		{D3D_PRIMITIVE_TOPOLOGY_POINTLIST, D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT}},
		{"LineList",
		{D3D_PRIMITIVE_TOPOLOGY_LINELIST, D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE}},
		{"TriangleList",
		{D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE}}
};

Topology::Topology(const std::string& topologyName){
	if (auto result = topologyMap.find(topologyName); result != topologyMap.end()) {
		m_IATopology = result->second.first;
		m_PSOTopology = result->second.second;
	}
	else
		throw GenericException("Invalid topology!");
}

void Topology::BindCommand(Graphics& gfx) noexcept {
	GetCommandList(gfx)->IASetPrimitiveTopology(m_IATopology);
}

D3D12_PRIMITIVE_TOPOLOGY_TYPE Topology::GetTopology() const noexcept {
	return m_PSOTopology;
}
