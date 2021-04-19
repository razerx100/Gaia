#ifndef __TEXTURE_HPP__
#define __TEXTURE_HPP__
#include <Bindable.hpp>

class Texture : public Bindable {
public:
	Texture(Graphics& gfx, const class Surface& s);
	~Texture() = default;

	void BindCommand(Graphics& gfx) noexcept override;
	void OnDestroy(Graphics& gfx) noexcept;

private:
	ComPtr<ID3D12DescriptorHeap> m_pSRVHeap;
	ComPtr<ID3D12Resource> m_pUploadHeap;
	ComPtr<ID3D12Resource> m_pTexture;

	std::uint32_t m_SRVIndex;
	D3D12_GPU_DESCRIPTOR_HANDLE m_GPUHandle;
};

#endif
