#ifndef __TOPOLOGY_HPP__
#define __TOPOLOGY_HPP__
#include <Bindable.hpp>

class Topology : public Bindable {
public:
	Topology(const std::string& topologyName);

	void BindCommand(Graphics& gfx) noexcept override;

	D3D12_PRIMITIVE_TOPOLOGY_TYPE GetTopology() const noexcept;

private:
	D3D_PRIMITIVE_TOPOLOGY m_IATopology;
	D3D12_PRIMITIVE_TOPOLOGY_TYPE m_PSOTopology;
};
#endif
