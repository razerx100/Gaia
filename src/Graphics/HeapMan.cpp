#include <HeapMan.hpp>
#include <Graphics.hpp>
#include <GraphicsThrowMacros.hpp>
#include <d3dx12.h>

HeapMan::HeapMan(D3D12_DESCRIPTOR_HEAP_TYPE type, Graphics& gfx)
    : m_CurrentDescCount(0u), m_NewDescCount(0u), m_GfxRef(gfx), m_HeapType(type), hr(0) {

    m_HeapIncrementSize = GetDevice(m_GfxRef)->GetDescriptorHandleIncrementSize(m_HeapType);


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
    if (m_AvailableDescs.empty())
        index = m_NewDescCount++;
    else {
        index = m_AvailableDescs.front();
        m_AvailableDescs.pop();
    }

    if (index < m_InUseDescs.size()) {
        m_InUseDescs[index] = true;
        m_InUseDescsCPUHandles[index] = cpuVisibleHandle;
        m_InUseDescsGPURefs[index] = setter;
    }
    else {
        m_InUseDescs.push_back(true);
        m_InUseDescsCPUHandles.push_back(cpuVisibleHandle);
        m_InUseDescsGPURefs.emplace_back(setter);
    }

    m_QueuedRequests.push(index);

    return index;
}

D3D12_CPU_DESCRIPTOR_HANDLE HeapMan::RequestHandleCPU(std::uint32_t handleIndex) {

    CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle(
        m_pGPUHeap->GetCPUDescriptorHandleForHeapStart(),
        handleIndex, m_HeapIncrementSize
    );

    return cpuHandle;
}

void HeapMan::Free(std::uint32_t index) {
    m_AvailableDescs.push(index);
    m_InUseDescs[index] = false;
}


void HeapMan::ProcessRequests() {
    if (m_NewDescCount > m_CurrentDescCount)
        CreateHeap(m_NewDescCount);

    for (int i = 0; i < m_QueuedRequests.size(); i++) {
        std::uint32_t index = m_QueuedRequests.front();
        m_QueuedRequests.pop();

        GetDevice(m_GfxRef)->CopyDescriptorsSimple(
            1u,
            CD3DX12_CPU_DESCRIPTOR_HANDLE(
                m_pGPUHeap->GetCPUDescriptorHandleForHeapStart(),
                index,
                m_HeapIncrementSize
            ),
            m_InUseDescsCPUHandles[index],
            m_HeapType
        );

        m_InUseDescsGPURefs[index].get() =
            CD3DX12_GPU_DESCRIPTOR_HANDLE(
                m_pGPUHeap->GetGPUDescriptorHandleForHeapStart(),
                index, m_HeapIncrementSize
            );
    }
}

ID3D12DescriptorHeap* HeapMan::GetHeap() const noexcept {
    return m_pGPUHeap.Get();
}

std::uint32_t HeapMan::GetHeapCount() const noexcept {
    return m_CurrentDescCount > 0;
}

void HeapMan::CreateHeap(std::uint32_t descriptorCount) {

    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
    heapDesc.NumDescriptors = descriptorCount;
    heapDesc.Type = m_HeapType;
    heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    GFX_THROW_FAILED(hr, GetDevice(m_GfxRef)->CreateDescriptorHeap(
        &heapDesc, __uuidof(ID3D12DescriptorHeap), &m_pGPUHeap
    ));

    m_CurrentDescCount = descriptorCount;

    for (int index = 0; index < m_InUseDescs.size(); index++) {
        if (m_InUseDescs[index]) {
            GetDevice(m_GfxRef)->CopyDescriptorsSimple(
                1u,
                CD3DX12_CPU_DESCRIPTOR_HANDLE(
                    m_pGPUHeap->GetCPUDescriptorHandleForHeapStart(),
                    index,
                    m_HeapIncrementSize
                ),
                m_InUseDescsCPUHandles[index],
                m_HeapType
            );

            m_InUseDescsGPURefs[index].get() =
                CD3DX12_GPU_DESCRIPTOR_HANDLE(
                    m_pGPUHeap->GetGPUDescriptorHandleForHeapStart(),
                    index, m_HeapIncrementSize
                );
        }
        else {
            GetDevice(m_GfxRef)->CopyDescriptorsSimple(
                1u,
                CD3DX12_CPU_DESCRIPTOR_HANDLE(
                    m_pGPUHeap->GetCPUDescriptorHandleForHeapStart(),
                    index,
                    m_HeapIncrementSize
                ),
                m_pEmptyCPUHeap->GetCPUDescriptorHandleForHeapStart(),
                m_HeapType
            );
        }

    }

}
