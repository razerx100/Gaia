#include <PipelineState.hpp>
#include <GraphicsThrowMacros.hpp>
#include <Graphics.hpp>
#include <PSODesc.hpp>

PipelineState::PipelineState(Graphics& gfx, const PSODesc& psoDesc) {
	GFX_THROW_FAILED(hr,
		GetDevice(gfx)->CreateGraphicsPipelineState(
				psoDesc.GetPSO(),
				__uuidof(ID3D12PipelineState),
				&m_pPipelineState
			)
		);

}

void PipelineState::BindCommand(Graphics& gfx) noexcept {
	GetCommandList(gfx)->SetPipelineState(m_pPipelineState.Get());
}
