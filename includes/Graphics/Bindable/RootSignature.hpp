#ifndef __ROOT_SIGNATURE_HPP__
#define __ROOT_SIGNATURE_HPP__
#include <Bindable.hpp>
#include <PreCompiled.hpp>
#include <vector>
#include <list>

class RootSignature : public Bindable {
public:
	RootSignature() = default;
	virtual ~RootSignature() = default;

	void CreateRootSignature(Graphics& gfx, ID3DBlob* signature);

	void BindCommand(Graphics& gfx) noexcept override;
	ID3D12RootSignature* GetSignature() const noexcept;

private:
	ComPtr<ID3D12RootSignature> m_pRootSignature;
};

class RootSignaturePreCompiled : public RootSignature, public PreCompiled {
public:
	RootSignaturePreCompiled(Graphics& gfx, const std::string& filename);
};

class RootSignatureDynamic : public RootSignature {
public:
	RootSignatureDynamic();

	void AddConstants(
		std::uint32_t numOfValues,
		D3D12_SHADER_VISIBILITY visibility,
		std::uint32_t registerNumber,
		std::uint32_t registerSpace = 0u
	) noexcept;
	void AddDescriptorTable(
		D3D12_DESCRIPTOR_RANGE_TYPE descriptorType,
		std::uint32_t descriptorsAmount,
		D3D12_SHADER_VISIBILITY visibility,
		std::uint32_t registerNumber,
		std::uint32_t registerSpace = 0u
	) noexcept;
	void AddConstantBufferView(
		D3D12_SHADER_VISIBILITY visibility,
		std::uint32_t registerNumber,
		std::uint32_t registerSpace = 0u
	) noexcept;
	void AddUnorderedAccessView(
		D3D12_SHADER_VISIBILITY visibility,
		std::uint32_t registerNumber,
		std::uint32_t registerSpace = 0u
	) noexcept;
	void AddStaticSampler() noexcept;
	void CompileSignature(Graphics& gfx);

private:
	ComPtr<ID3DBlob> m_pBinary;
	std::vector<D3D12_ROOT_PARAMETER1> m_rootParameters;
	std::list<D3D12_DESCRIPTOR_RANGE1> m_descriptorRanges;

	bool m_hasStaticSampler;
};
#endif
