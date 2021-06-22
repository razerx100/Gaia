#include <BindableHeap.hpp>
#include <GraphicsThrowMacros.hpp>
#include <HeapMan.hpp>
#include <d3dx12.h>
#include <ResourceView.hpp>
#include <Texture.hpp>

BindableHeap::BindableHeap(Graphics& gfx,
	std::uint32_t rootIndex,
	std::uint32_t descriptorAmount)
	: m_rootIndex(rootIndex) {

	m_heapIncrementSize = GetDevice(gfx)->GetDescriptorHandleIncrementSize(
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
	);

	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = descriptorAmount;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	GFX_THROW_FAILED(hr, GetDevice(gfx)->CreateDescriptorHeap(
		&srvHeapDesc, __uuidof(ID3D12DescriptorHeap), &m_pHeap
	));

	m_srvMainHeapIndex = GetSRVHeapMan(gfx).RequestHandleIndex(
		m_pHeap->GetCPUDescriptorHandleForHeapStart(),
		&m_gpuHandle,
		descriptorAmount
	);
}

void BindableHeap::AddSRV(Graphics& gfx, std::unique_ptr<Texture> texture) {
	texture->CreateSRV(gfx, GetCpuHandle(texture->GetDescriptorIndex()));

	m_resourceViews.emplace_back(std::move(texture));
}

D3D12_CPU_DESCRIPTOR_HANDLE BindableHeap::GetCpuHandle(
	std::uint32_t descriptorIndex
) const noexcept {

	return CD3DX12_CPU_DESCRIPTOR_HANDLE(
		m_pHeap->GetCPUDescriptorHandleForHeapStart(),
		descriptorIndex,
		m_heapIncrementSize
	);
}

void BindableHeap::OnDestroy(Graphics& gfx) noexcept {
	GetSRVHeapMan(gfx).Free(m_srvMainHeapIndex);
}

void BindableHeap::BindCommand(Graphics& gfx) noexcept {
	GetCommandList(gfx)->SetGraphicsRootDescriptorTable(
		m_rootIndex, m_gpuHandle
	);
}
