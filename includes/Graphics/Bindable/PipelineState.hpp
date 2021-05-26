#ifndef __PIPELINE_STATE_HPP__
#define __PIPELINE_STATE_HPP__
#include <Bindable.hpp>
#include <PSODesc.hpp>

class PipelineState : public Bindable {
public:
	PipelineState(Graphics& gfx, const PSODesc& psoDesc);

	void BindCommand(Graphics& gfx) noexcept override;
private:
	ComPtr<ID3D12PipelineState> m_pPipelineState;
};
#endif
