#ifndef __PSO_DESC_HPP__
#define __PSO_DESC_HPP__
#include <CleanWin.hpp>
#include <d3d12.h>
#include <string>
#include <PipelineObjects.hpp>

class PSODesc {
public:
	PSODesc();

	void SetInputLayout(const VertexLayout& vertexLayout) noexcept;
	void SetRootSignature(class RootSignature* signature) noexcept;
	void SetVertexShader(const std::string& filePath) noexcept;
	void SetPixelShader(const std::string& filePath) noexcept;
	void SetTopology(class Topology* topology) noexcept;

	const D3D12_GRAPHICS_PIPELINE_STATE_DESC* GetPSO() const noexcept;

private:
	D3D12_GRAPHICS_PIPELINE_STATE_DESC m_psoDesc;
	InputLayout m_inputLayout;
	Shader m_vertexShader;
	Shader m_pixelShader;
};

std::string GeneratePSOName(
	const std::string& layoutName, const std::string& RSName,
	const std::string& VSName, const std::string& PSName,
	const std::string& topologyName
) noexcept;
#endif
