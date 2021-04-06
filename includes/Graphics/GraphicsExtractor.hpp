#ifndef __GRAPHICS_EXTRACTOR_HPP__
#define __GRAPHICS_EXTRACTOR_HPP__
#include <CleanWin.hpp>
#include <d3d12.h>

class Graphics;

class GraphicsExtractor {
protected:
	static ID3D12Device2* GetDevice(Graphics& gfx) noexcept;
	static ID3D12GraphicsCommandList* GetCommandList(Graphics& gfx) noexcept;
	static ID3D12DescriptorHeap* GetSRVHeap(Graphics& gfx) noexcept;
};
#endif
