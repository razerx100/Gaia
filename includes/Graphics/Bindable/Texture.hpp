#ifndef __TEXTURE_HPP__
#define __TEXTURE_HPP__
#include <Bindable.hpp>

class Texture : public Bindable {
public:
	Texture(Graphics& gfx, const class Surface& s);

	void BindCommand(Graphics& gfx) noexcept override;

private:
	ComPtr<ID3D12DescriptorHeap> m_pSRVHeap;
	ComPtr<ID3D12Resource> m_pUploadHeap;
	ComPtr<ID3D12Resource> m_pTexture;
};

#endif
