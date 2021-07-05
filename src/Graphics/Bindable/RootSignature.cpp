#include <RootSignature.hpp>
#include <GraphicsThrowMacros.hpp>
#include <Graphics.hpp>
#include <d3dx12.h>
#include <d3dcompiler.h>

void RootSignature::CreateRootSignature(Graphics& gfx, ID3DBlob* signature) {
	GFX_THROW_FAILED(hr,
		GetDevice(gfx)->CreateRootSignature(
			0u,
			signature->GetBufferPointer(),
			signature->GetBufferSize(),
			__uuidof(ID3D12RootSignature),
			&m_pRootSignature
		)
	);
}

void RootSignature::BindCommand(Graphics& gfx) noexcept {
	GetCommandList(gfx)->SetGraphicsRootSignature(m_pRootSignature.Get());
}

ID3D12RootSignature* RootSignature::GetSignature() const noexcept {
	return m_pRootSignature.Get();
}


RootSignaturePreCompiled::RootSignaturePreCompiled(
	Graphics& gfx, const std::wstring& filename
) : PreCompiled(filename) {
	CreateRootSignature(gfx, m_pBinary.Get());
}


RootSignatureDynamic::RootSignatureDynamic()
	: m_hasStaticSampler(false) {}

void RootSignatureDynamic::CompileSignature(Graphics& gfx) {
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSigDesc = {};
	D3D12_ROOT_SIGNATURE_FLAGS sigFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;

	if (m_hasStaticSampler) {
		D3D12_STATIC_SAMPLER_DESC staticSamplerDesc = {};
		staticSamplerDesc.ShaderRegister = 0u;
		staticSamplerDesc.RegisterSpace = 0u;
		staticSamplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		staticSamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSamplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;

		rootSigDesc.Init_1_1(
			static_cast<std::uint32_t>(m_rootParameters.size()),
			m_rootParameters.data(),
			1u,
			&staticSamplerDesc,
			sigFlags
		);
	}
	else
		rootSigDesc.Init_1_1(
			static_cast<std::uint32_t>(m_rootParameters.size()),
			m_rootParameters.data(),
			0u,
			nullptr,
			sigFlags
		);

	ComPtr<ID3DBlob> error;
	GFX_THROW_FAILED(hr,
		D3DX12SerializeVersionedRootSignature(
			&rootSigDesc,
			D3D_ROOT_SIGNATURE_VERSION_1_1,
			&m_pBinary,
			&error
		)
	);

	m_rootParameters = std::vector<D3D12_ROOT_PARAMETER1>();
	m_descriptorRanges = std::list<D3D12_DESCRIPTOR_RANGE1>();

	CreateRootSignature(gfx, m_pBinary.Get());
}

void RootSignatureDynamic::AddConstants(
	std::uint32_t numOfValues,
	D3D12_SHADER_VISIBILITY visibility,
	std::uint32_t registerNumber,
	std::uint32_t registerSpace
) noexcept {
	CD3DX12_ROOT_PARAMETER1 rootParam = {};
	rootParam.InitAsConstants(
		numOfValues,
		registerNumber,
		registerSpace,
		visibility
	);

	m_rootParameters.emplace_back(rootParam);
}

void RootSignatureDynamic::AddStaticSampler() noexcept {
	m_hasStaticSampler = true;
}

void RootSignatureDynamic::AddDescriptorTable(
	D3D12_DESCRIPTOR_RANGE_TYPE descriptorType,
	std::uint32_t descriptorsAmount,
	D3D12_SHADER_VISIBILITY visibility,
	std::uint32_t registerNumber,
	std::uint32_t registerSpace
) noexcept {
	CD3DX12_DESCRIPTOR_RANGE1 descRange = {};
	descRange.Init(
		descriptorType, descriptorsAmount, registerNumber, registerSpace,
		D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE
	);

	// Range's address needs to be preserved till Signature Desc init
	m_descriptorRanges.emplace_back(descRange);

	CD3DX12_ROOT_PARAMETER1 rootParam = {};
	rootParam.InitAsDescriptorTable(1, &m_descriptorRanges.back(), visibility);

	m_rootParameters.emplace_back(rootParam);
}

void RootSignatureDynamic::AddConstantBufferView(
	D3D12_SHADER_VISIBILITY visibility,
	std::uint32_t registerNumber,
	std::uint32_t registerSpace
) noexcept {
	CD3DX12_ROOT_PARAMETER1 rootParam = {};
	rootParam.InitAsConstantBufferView(
		registerNumber, registerSpace,
		D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE,
		visibility
	);

	m_rootParameters.emplace_back(rootParam);
}

void RootSignatureDynamic::AddUnorderedAccessView(
	D3D12_SHADER_VISIBILITY visibility,
	std::uint32_t registerNumber,
	std::uint32_t registerSpace
) noexcept {
	CD3DX12_ROOT_PARAMETER1 rootParam = {};
	rootParam.InitAsUnorderedAccessView(
		registerNumber, registerSpace,
		D3D12_ROOT_DESCRIPTOR_FLAG_DATA_VOLATILE,
		visibility
	);

	m_rootParameters.emplace_back(rootParam);
}
