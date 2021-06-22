#include <HeapMan.hpp>
#include <Graphics.hpp>
#include <GraphicsThrowMacros.hpp>
#include <d3dx12.h>

HeapMan::HeapMan(D3D12_DESCRIPTOR_HEAP_TYPE type, Graphics* gfx)
    : m_currentDescCount(0u), m_currentDescOffsetFromBase(0u),
    m_gfxRef(gfx),
    m_heapType(type), hr(0) {

    m_heapIncrementSize = GetDevice(*m_gfxRef)->GetDescriptorHandleIncrementSize(
        m_heapType
    );
}

std::optional<std::uint32_t> HeapMan::FindPartitionIndex(
    std::uint32_t descCount
) noexcept {
    for (auto index = m_freeDescTables.begin();
        index != m_freeDescTables.end(); ++index
        ) {
        if (m_descTableDescCounts[*index] > descCount) {
            std::uint32_t newIndex = CreateNewTable(
                m_descTableDescCounts[*index] - descCount,
                m_descTableOffsetsFromBase[*index] + descCount
            );
            m_freeDescTables.emplace_back(
                newIndex
            );

            m_descTableDescCounts[*index] = descCount;
            m_freeDescTables.erase(index);
            return *index;
        }
    }

    return std::nullopt;
}

std::uint32_t HeapMan::CreateNewTable(
    std::uint32_t descriptorCount,
    std::optional<std::uint32_t> offsetFromBase
) noexcept {
    std::uint32_t index = static_cast<std::uint32_t>(
        m_descTableOffsetsFromBase.size()
        );

    m_descTableDescCounts.emplace_back(descriptorCount);
    m_descTableCPUHandles.emplace_back();
    m_descTableGPURefs.emplace_back();

    if (offsetFromBase)
        m_descTableOffsetsFromBase.emplace_back(*offsetFromBase);
    else {
        m_descTableOffsetsFromBase.emplace_back(
            m_currentDescOffsetFromBase
        );

        m_currentDescOffsetFromBase += descriptorCount;
    }

    return index;
}

std::uint32_t HeapMan::RequestHandleIndex(
    D3D12_CPU_DESCRIPTOR_HANDLE cpuVisibleHandle,
    D3D12_GPU_DESCRIPTOR_HANDLE* setter,
    std::uint32_t descriptorCount
) {
    std::uint32_t index;
    if (auto qIndex = FindPartitionIndex(descriptorCount))
        index = *qIndex;
    else
        index = CreateNewTable(descriptorCount);

    m_descTableCPUHandles[index] = cpuVisibleHandle;
    m_descTableGPURefs[index] = setter;

    m_queuedRequests.push(index);

    return index;
}

D3D12_CPU_DESCRIPTOR_HANDLE HeapMan::RequestHandleCPU(std::uint32_t descTableIndex) {
    return CD3DX12_CPU_DESCRIPTOR_HANDLE(
        m_pGPUHeap->GetCPUDescriptorHandleForHeapStart(),
        m_descTableOffsetsFromBase[descTableIndex], m_heapIncrementSize
    );
}

void HeapMan::Free(std::uint32_t index) {
    m_inUseDescTables.remove(index);
    m_freeDescTables.emplace_back(index);
}

void HeapMan::CopyDescriptors(std::uint32_t tableIndex) {
    GetDevice(*m_gfxRef)->CopyDescriptorsSimple(
        m_descTableDescCounts[tableIndex],
        CD3DX12_CPU_DESCRIPTOR_HANDLE(
            m_pGPUHeap->GetCPUDescriptorHandleForHeapStart(),
            m_descTableOffsetsFromBase[tableIndex],
            m_heapIncrementSize
        ),
        m_descTableCPUHandles[tableIndex],
        m_heapType
    );

    *m_descTableGPURefs[tableIndex] =
        CD3DX12_GPU_DESCRIPTOR_HANDLE(
            m_pGPUHeap->GetGPUDescriptorHandleForHeapStart(),
            m_descTableOffsetsFromBase[tableIndex], m_heapIncrementSize
        );
}

void HeapMan::ProcessRequests() {
    if (m_currentDescOffsetFromBase > m_currentDescCount)
        CreateHeap(m_currentDescOffsetFromBase);

    int requestsSize = m_queuedRequests.size();

    for (int i = 0; i < requestsSize; i++) {
        std::uint32_t index = m_queuedRequests.front();
        m_queuedRequests.pop();

        CopyDescriptors(index);
        m_inUseDescTables.emplace_back(index);
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

    GFX_THROW_FAILED(hr, GetDevice(*m_gfxRef)->CreateDescriptorHeap(
        &heapDesc, __uuidof(ID3D12DescriptorHeap), &m_pGPUHeap
    ));

    m_currentDescCount = descriptorCount;

    for (std::uint32_t index : m_inUseDescTables)
        CopyDescriptors(index);
}
