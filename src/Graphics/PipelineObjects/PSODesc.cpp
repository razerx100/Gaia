#include <PSODesc.hpp>
#include <d3dx12.h>
#include <Graphics.hpp>
#include <RootSignature.hpp>
#include <Topology.hpp>

PSODesc::PSODesc()
	: m_psoDesc{} {
	m_psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	m_psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	m_psoDesc.DepthStencilState.DepthEnable = TRUE;
	m_psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	m_psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	m_psoDesc.DepthStencilState.StencilEnable = FALSE;
	m_psoDesc.SampleMask = UINT_MAX;
	m_psoDesc.NumRenderTargets = 1;
	m_psoDesc.RTVFormats[0] = Graphics::GetRenderFormat();
	m_psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	m_psoDesc.SampleDesc.Count = 1;
}

void PSODesc::SetInputLayout(const VertexLayout& vertexLayout) noexcept {
	m_inputLayout.Init(vertexLayout);

	m_psoDesc.InputLayout = m_inputLayout.GetInputLayout();
}

void PSODesc::SetRootSignature(RootSignature* signature) noexcept {
	m_psoDesc.pRootSignature = signature->GetSignature();
}

void PSODesc::SetVertexShader(const std::string& filePath) noexcept {
	m_vertexShader.Init(filePath);

	m_psoDesc.VS = m_vertexShader.GetShaderByteCode();
}

void PSODesc::SetPixelShader(const std::string& filePath) noexcept {
	m_pixelShader.Init(filePath);

	m_psoDesc.PS = m_pixelShader.GetShaderByteCode();
}

void PSODesc::SetTopology(Topology* topology) noexcept {
	m_psoDesc.PrimitiveTopologyType = topology ->GetTopology();
}

const D3D12_GRAPHICS_PIPELINE_STATE_DESC* PSODesc::GetPSO() const noexcept {
	return &m_psoDesc;
}

std::string GeneratePSOName(
	const std::string& layoutName, const std::string& RSName,
	const std::string& VSName, const std::string& PSName,
	const std::string& topologyName
) noexcept {
	return topologyName + "#" + layoutName + RSName + "#" + VSName + "#" + PSName;
}
