#include <HeapMan.hpp>
#include <Graphics.hpp>
#include <GraphicsThrowMacros.hpp>
#include <d3dx12.h>

HeapMan::HeapMan(D3D12_DESCRIPTOR_HEAP_TYPE type, Graphics& gfx)
    : m_CurrentDescCount(0u), m_NewDescCount(0u), m_GfxRef(gfx), m_HeapType(type), hr(0) {

    m_HeapIncrementSize = GetDevice(m_GfxRef)->GetDescriptorHandleIncrementSize(m_HeapType);

}

std::uint32_t HeapMan::RequestHandleIndex(D3D12_CPU_DESCRIPTOR_HANDLE cpuVisibleHandle) {
    std::uint32_t index;
    if (m_AvailableDescs.empty())
        index = m_NewDescCount++;
    else {
        index = m_AvailableDescs.front();
        m_AvailableDescs.pop();
    }

    m_QueuedRequests.push({ cpuVisibleHandle, index });

    return index;
}

D3D12_GPU_DESCRIPTOR_HANDLE HeapMan::RequestHandle(std::uint32_t handleIndex) {

    CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle(
        m_pGPUHeap->GetGPUDescriptorHandleForHeapStart(),
        handleIndex, m_HeapIncrementSize
    );

    return gpuHandle;
}

void HeapMan::Free(std::uint32_t index) {
    m_AvailableDescs.push(index);
    m_InUseDescs.erase(index);
}


void HeapMan::ProcessRequests() {
    if (m_NewDescCount > m_CurrentDescCount) {
        CreateHeap(m_NewDescCount);

        for (int i = 0; i < m_InUseDescs.size(); i++) {
            GetDevice(m_GfxRef)->CopyDescriptorsSimple(
                    1u,
                    CD3DX12_CPU_DESCRIPTOR_HANDLE(
                        m_pGPUHeap->GetCPUDescriptorHandleForHeapStart(),
                        i,
                        m_HeapIncrementSize
                    ),
                    m_InUseDescs[i],
                    m_HeapType
                );
        }
    }

    for (int i = 0; i < m_QueuedRequests.size(); i++) {
        std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, std::uint32_t> pair = m_QueuedRequests.front();
        m_QueuedRequests.pop();

        m_InUseDescs.insert_or_assign(pair.second, pair.first);

        GetDevice(m_GfxRef)->CopyDescriptorsSimple(
            1u,
            CD3DX12_CPU_DESCRIPTOR_HANDLE(
                m_pGPUHeap->GetCPUDescriptorHandleForHeapStart(),
                pair.second,
                m_HeapIncrementSize
            ),
            pair.first,
            m_HeapType
        );
    }
}

ID3D12DescriptorHeap*const* HeapMan::GetHeap() const {
    return m_pGPUHeap.GetAddressOf();
}

void HeapMan::CreateHeap(std::uint32_t descriptorCount) {

    D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
    srvHeapDesc.NumDescriptors = descriptorCount;
    srvHeapDesc.Type = m_HeapType;
    srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    GFX_THROW_FAILED(hr, GetDevice(m_GfxRef)->CreateDescriptorHeap(
        &srvHeapDesc, __uuidof(ID3D12DescriptorHeap), &m_pGPUHeap
    ));

    m_CurrentDescCount = descriptorCount;
}
