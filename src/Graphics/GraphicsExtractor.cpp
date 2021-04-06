#include <GraphicsExtractor.hpp>
#include <Graphics.hpp>

ID3D12Device2* GraphicsExtractor::GetDevice(Graphics& gfx) noexcept {
	return gfx.m_pDevice.Get();
}

ID3D12GraphicsCommandList* GraphicsExtractor::GetCommandList(Graphics& gfx) noexcept {
	return gfx.m_pCommandList.Get();
}

ID3D12DescriptorHeap* GraphicsExtractor::GetSRVHeap(Graphics& gfx) noexcept {
	return gfx.m_pSRVHeap.Get();
}
