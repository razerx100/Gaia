#include <PipelineState.hpp>
#include <GraphicsThrowMacros.hpp>

PipelineState::PipelineState(Graphics& gfx, const D3D12_GRAPHICS_PIPELINE_STATE_DESC* psoDesc) {
	GFX_THROW_FAILED(hr,
		GetDevice(gfx)->CreateGraphicsPipelineState(
				psoDesc,
				__uuidof(ID3D12PipelineState),
				&m_pPipelineState
			)
		);
}

void PipelineState::BindCommand(Graphics& gfx) noexcept {
	GetCommandList(gfx)->SetPipelineState(m_pPipelineState.Get());
}
