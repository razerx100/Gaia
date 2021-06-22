#include <HeapMan.hpp>
#include <Graphics.hpp>
#include <GraphicsThrowMacros.hpp>
#include <d3dx12.h>

HeapMan::HeapMan(D3D12_DESCRIPTOR_HEAP_TYPE type, Graphics& gfx)
    : m_currentDescCount(0u), m_newDescCount(0u), m_gfxRef(gfx),
    m_heapType(type), hr(0) {

    m_heapIncrementSize = GetDevice(m_gfxRef)->GetDescriptorHandleIncrementSize(
        m_heapType
    );

    // Empty Heap to replace removed heaps in the main heap
    D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
    srvHeapDesc.NumDescriptors = 1;
    srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    GetDevice(gfx)->CreateDescriptorHeap(
        &srvHeapDesc, __uuidof(ID3D12DescriptorHeap), &m_pEmptyCPUHeap
    );
}

std::uint32_t HeapMan::RequestHandleIndex(
    D3D12_CPU_DESCRIPTOR_HANDLE cpuVisibleHandle,
    D3D12_GPU_DESCRIPTOR_HANDLE& setter
) {
    std::uint32_t index;
    if (m_availableDescs.empty())
        index = m_newDescCount++;
    else {
        index = m_availableDescs.front();
        m_availableDescs.pop();
    }

    if (index < m_inUseDescs.size()) {
        m_inUseDescs[index] = true;
        m_inUseDescsCPUHandles[index] = cpuVisibleHandle;
        m_inUseDescsGPURefs[index] = setter;
    }
    else {
        m_inUseDescs.push_back(true);
        m_inUseDescsCPUHandles.push_back(cpuVisibleHandle);
        m_inUseDescsGPURefs.emplace_back(setter);
    }

    m_queuedRequests.push(index);

    return index;
}

D3D12_CPU_DESCRIPTOR_HANDLE HeapMan::RequestHandleCPU(std::uint32_t handleIndex) {
    return CD3DX12_CPU_DESCRIPTOR_HANDLE(
        m_pGPUHeap->GetCPUDescriptorHandleForHeapStart(),
        handleIndex, m_heapIncrementSize
    );
}

void HeapMan::Free(std::uint32_t index) {
    m_availableDescs.push(index);
    m_inUseDescs[index] = false;
}


void HeapMan::ProcessRequests() {
    if (m_newDescCount > m_currentDescCount)
        CreateHeap(m_newDescCount);

    for (int i = 0; i < m_queuedRequests.size(); i++) {
        std::uint32_t index = m_queuedRequests.front();
        m_queuedRequests.pop();

        GetDevice(m_gfxRef)->CopyDescriptorsSimple(
            1u,
            CD3DX12_CPU_DESCRIPTOR_HANDLE(
                m_pGPUHeap->GetCPUDescriptorHandleForHeapStart(),
                index,
                m_heapIncrementSize
            ),
            m_inUseDescsCPUHandles[index],
            m_heapType
        );

        m_inUseDescsGPURefs[index].get() =
            CD3DX12_GPU_DESCRIPTOR_HANDLE(
                m_pGPUHeap->GetGPUDescriptorHandleForHeapStart(),
                index, m_heapIncrementSize
            );
    }
}

ID3D12DescriptorHeap* HeapMan::GetHeap() const noexcept {
    return m_pGPUHeap.Get();
}

std::uint32_t HeapMan::GetHeapCount() const noexcept {
    return m_currentDescCount > 0;
}

void HeapMan::CreateHeap(std::uint32_t descriptorCount) {

    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
    heapDesc.NumDescriptors = descriptorCount;
    heapDesc.Type = m_heapType;
    heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    GFX_THROW_FAILED(hr, GetDevice(m_gfxRef)->CreateDescriptorHeap(
        &heapDesc, __uuidof(ID3D12DescriptorHeap), &m_pGPUHeap
    ));

    m_currentDescCount = descriptorCount;

    for (int index = 0; index < m_inUseDescs.size(); index++) {
        if (m_inUseDescs[index]) {
            GetDevice(m_gfxRef)->CopyDescriptorsSimple(
                1u,
                CD3DX12_CPU_DESCRIPTOR_HANDLE(
                    m_pGPUHeap->GetCPUDescriptorHandleForHeapStart(),
                    index,
                    m_heapIncrementSize
                ),
                m_inUseDescsCPUHandles[index],
                m_heapType
            );

            m_inUseDescsGPURefs[index].get() =
                CD3DX12_GPU_DESCRIPTOR_HANDLE(
                    m_pGPUHeap->GetGPUDescriptorHandleForHeapStart(),
                    index, m_heapIncrementSize
                );
        }
        else {
            GetDevice(m_gfxRef)->CopyDescriptorsSimple(
                1u,
                CD3DX12_CPU_DESCRIPTOR_HANDLE(
                    m_pGPUHeap->GetCPUDescriptorHandleForHeapStart(),
                    index,
                    m_heapIncrementSize
                ),
                m_pEmptyCPUHeap->GetCPUDescriptorHandleForHeapStart(),
                m_heapType
            );
        }

    }

}
