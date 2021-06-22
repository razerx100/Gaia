#ifndef __BINDABLE_HEAP_HPP__
#define __BINDABLE_HEAP_HPP__
#include <Bindable.hpp>
#include <cinttypes>
#include <vector>
#include <memory>

class BindableHeap : public Bindable {
public:
	BindableHeap(Graphics& gfx,
		std::uint32_t rootIndex,
		std::uint32_t descriptorAmount);

	void AddSRV(Graphics& gfx, std::unique_ptr<class Texture> texture);

	void BindCommand(Graphics& gfx) noexcept override;
	void OnDestroy(Graphics& gfx) noexcept;

private:
	D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(
		std::uint32_t descriptorIndex
	) const noexcept;

private:
	ComPtr<ID3D12DescriptorHeap> m_pHeap;
	std::vector<std::unique_ptr<class ResourceView>> m_resourceViews;

	std::uint32_t m_rootIndex;
	std::uint32_t m_srvMainHeapIndex;
	std::uint32_t m_heapIncrementSize;
	D3D12_GPU_DESCRIPTOR_HANDLE m_gpuHandle;
};
#endif
