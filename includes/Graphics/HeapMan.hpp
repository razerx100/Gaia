#ifndef __HEAP_MAN_HPP__
#define __HEAP_MAN_HPP__
#include <GraphicsExtractor.hpp>
#include <wrl.h>
#include <unordered_map>
#include <queue>

using Microsoft::WRL::ComPtr;

class HeapMan : public GraphicsExtractor {
public:
	HeapMan(D3D12_DESCRIPTOR_HEAP_TYPE type, Graphics& gfx);

	std::uint32_t RequestHandleIndex(D3D12_CPU_DESCRIPTOR_HANDLE cpuVisibleHandle);
	D3D12_GPU_DESCRIPTOR_HANDLE RequestHandle(std::uint32_t handleIndex);

	void Free(std::uint32_t index);
	void ProcessRequests();

	ID3D12DescriptorHeap*const* GetHeap() const;

private:
	void CreateHeap(std::uint32_t descriptorCount);

private:
	ComPtr<ID3D12DescriptorHeap> m_pGPUHeap;

	std::unordered_map<std::uint32_t, D3D12_CPU_DESCRIPTOR_HANDLE> m_InUseDescs;
	std::queue<std::uint32_t> m_AvailableDescs;

	std::queue<std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, std::uint32_t>> m_QueuedRequests;

	std::uint32_t m_CurrentDescCount;
	std::uint32_t m_HeapIncrementSize;
	std::uint32_t m_NewDescCount;

	Graphics& m_GfxRef;

	const D3D12_DESCRIPTOR_HEAP_TYPE m_HeapType;

	HRESULT hr;
};
#endif
