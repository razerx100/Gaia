#ifndef __TEXTURE_HPP__
#define __TEXTURE_HPP__
#include <GraphicsExtractor.hpp>
#include <ResourceView.hpp>
#include <wrl.h>

using Microsoft::WRL::ComPtr;

class Texture : public GraphicsExtractor, public ResourceView {
public:
	Texture(Graphics& gfx,
		const class Surface& s,
		std::uint32_t descriptorIndex);
	~Texture() = default;

	void CreateSRV(Graphics& gfx, D3D12_CPU_DESCRIPTOR_HANDLE srvHandle);

private:
	ComPtr<ID3D12Resource> m_pUploadHeap;
	ComPtr<ID3D12Resource> m_pTexture;
};

#endif
