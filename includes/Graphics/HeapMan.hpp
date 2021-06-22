#ifndef __HEAP_MAN_HPP__
#define __HEAP_MAN_HPP__
#include <GraphicsExtractor.hpp>
#include <wrl.h>
#include <queue>
#include <list>
#include <functional>
#include <optional>

using Microsoft::WRL::ComPtr;

class HeapMan : public GraphicsExtractor {
public:
	HeapMan(D3D12_DESCRIPTOR_HEAP_TYPE type, Graphics* gfx);

	std::uint32_t RequestHandleIndex(
		D3D12_CPU_DESCRIPTOR_HANDLE cpuVisibleHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE* setter,
		std::uint32_t descriptorCount = 1u
	);

	D3D12_CPU_DESCRIPTOR_HANDLE RequestHandleCPU(std::uint32_t descTableIndex);

	void Free(std::uint32_t index);
	void ProcessRequests();

	ID3D12DescriptorHeap* GetHeap() const noexcept;
	std::uint32_t GetHeapCount() const noexcept;

private:
	void CreateHeap(std::uint32_t descriptorCount);
	void CopyDescriptors(std::uint32_t tableIndex);

	std::uint32_t CreateNewTable(
		std::uint32_t descriptorCount,
		std::optional<std::uint32_t> offsetFromBase = std::nullopt
	) noexcept;
	std::optional<std::uint32_t> FindPartitionIndex(
		std::uint32_t descCount
	) noexcept;

private:
	ComPtr<ID3D12DescriptorHeap> m_pGPUHeap;

	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> m_descTableCPUHandles;
	std::vector<D3D12_GPU_DESCRIPTOR_HANDLE*> m_descTableGPURefs;
	std::vector<std::uint32_t> m_descTableDescCounts;
	std::vector<std::uint32_t> m_descTableOffsetsFromBase;

	// use vector instead
	std::list<std::uint32_t> m_freeDescTables;
	std::list<std::uint32_t> m_inUseDescTables;
	std::queue<std::uint32_t> m_queuedRequests;

	std::uint32_t m_currentDescCount;
	std::uint32_t m_currentDescOffsetFromBase;
	std::uint32_t m_heapIncrementSize;

	Graphics* m_gfxRef;
	const D3D12_DESCRIPTOR_HEAP_TYPE m_heapType;
	HRESULT hr;
};
#endif
