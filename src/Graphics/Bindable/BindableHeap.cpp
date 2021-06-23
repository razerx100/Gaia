#include <BindableHeap.hpp>
#include <GraphicsThrowMacros.hpp>
#include <HeapMan.hpp>
#include <d3dx12.h>
#include <ResourceView.hpp>
#include <Texture.hpp>

BindableHeap::BindableHeap(Graphics& gfx,
	std::uint32_t rootIndex)
	: m_rootIndex(rootIndex) {

	m_heapIncrementSize = GetDevice(gfx)->GetDescriptorHandleIncrementSize(
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
	);
}

void BindableHeap::FinishedAdding(Graphics& gfx) {
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = static_cast<std::uint32_t>(m_copyHeaps.size());
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	GFX_THROW_FAILED(hr, GetDevice(gfx)->CreateDescriptorHeap(
		&srvHeapDesc, __uuidof(ID3D12DescriptorHeap), &m_pHeap
	));

	for (std::uint32_t i = 0; i < m_copyHeaps.size(); ++i)
		GetDevice(gfx)->CopyDescriptorsSimple(
			1,
			CD3DX12_CPU_DESCRIPTOR_HANDLE(
				m_pHeap->GetCPUDescriptorHandleForHeapStart(),
				i,
				m_heapIncrementSize
			),
			m_copyHeaps[i]->GetCPUDescriptorHandleForHeapStart(),
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
		);

	m_srvMainHeapIndex = GetSRVHeapMan(gfx).RequestHandleIndex(
		m_pHeap->GetCPUDescriptorHandleForHeapStart(),
		&m_gpuHandle,
		static_cast<std::uint32_t>(m_copyHeaps.size())
	);

	m_copyHeaps = std::vector<ComPtr<ID3D12DescriptorHeap>>();
}

void BindableHeap::AddSRV(Graphics& gfx, std::unique_ptr<Texture> texture) {
	ComPtr<ID3D12DescriptorHeap> copyHeap;

	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = 1;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	GFX_THROW_FAILED(hr, GetDevice(gfx)->CreateDescriptorHeap(
		&srvHeapDesc, __uuidof(ID3D12DescriptorHeap), &copyHeap
	));

	texture->CreateSRV(gfx, copyHeap->GetCPUDescriptorHandleForHeapStart());

	m_copyHeaps.emplace_back(copyHeap);
	m_resourceViews.emplace_back(std::move(texture));
}

void BindableHeap::OnDestroy(Graphics& gfx) noexcept {
	GetSRVHeapMan(gfx).Free(m_srvMainHeapIndex);
}

void BindableHeap::BindCommand(Graphics& gfx) noexcept {
	GetCommandList(gfx)->SetGraphicsRootDescriptorTable(
		m_rootIndex, m_gpuHandle
	);
}
