#ifndef __PSO_DESC_HPP__
#define __PSO_DESC_HPP__
#include <CleanWin.hpp>
#include <d3d12.h>
#include <PipelineObjects.hpp>
#include <RootSignature.hpp>
#include <Topology.hpp>

class PSODesc {
public:
	PSODesc();

	void SetInputLayout(std::vector<D3D12_INPUT_ELEMENT_DESC>&& inputElements) noexcept;
	void SetRootSignature(RootSignature* signature) noexcept;
	void SetVertexShader(const std::wstring& filePath) noexcept;
	void SetPixelShader(const std::wstring& filePath) noexcept;
	void SetTopology(Topology* topology) noexcept;

	const D3D12_GRAPHICS_PIPELINE_STATE_DESC* GetPSO() const noexcept;

private:
	D3D12_GRAPHICS_PIPELINE_STATE_DESC m_psoDesc;
	InputLayout m_inputLayout;
	Shader m_vertexShader;
	Shader m_pixelShader;
};
#endif
