#ifndef __HEAP_MAN_HPP__
#define __HEAP_MAN_HPP__
#include <GraphicsExtractor.hpp>
#include <wrl.h>
#include <queue>
#include <functional>

using Microsoft::WRL::ComPtr;

class HeapMan : public GraphicsExtractor {
public:
	HeapMan(D3D12_DESCRIPTOR_HEAP_TYPE type, Graphics& gfx);

	std::uint32_t RequestHandleIndex(
		D3D12_CPU_DESCRIPTOR_HANDLE cpuVisibleHandle,
		std::function<void(D3D12_GPU_DESCRIPTOR_HANDLE)> setter
		);

	D3D12_CPU_DESCRIPTOR_HANDLE RequestHandleCPU(std::uint32_t handleIndex);

	void Free(std::uint32_t index);
	void ProcessRequests();

	ID3D12DescriptorHeap* GetHeap() const;

private:
	void CreateHeap(std::uint32_t descriptorCount);

private:
	ComPtr<ID3D12DescriptorHeap> m_pGPUHeap;
	ComPtr<ID3D12DescriptorHeap> m_pEmptyCPUHeap;

	std::vector<bool> m_InUseDescs;
	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> m_InUseDescsCPUHandles;
	std::vector<std::function<void(D3D12_GPU_DESCRIPTOR_HANDLE)>> m_InUseDescsSetters;

	std::queue<std::uint32_t> m_AvailableDescs;

	std::queue<std::uint32_t> m_QueuedRequests;

	std::uint32_t m_CurrentDescCount;
	std::uint32_t m_HeapIncrementSize;
	std::uint32_t m_NewDescCount;

	Graphics& m_GfxRef;

	const D3D12_DESCRIPTOR_HEAP_TYPE m_HeapType;

	HRESULT hr;
};
#endif
