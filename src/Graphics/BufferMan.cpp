#include <BufferMan.hpp>
#include <d3dx12.h>
#include <GraphicsThrowMacros.hpp>
#include <Graphics.hpp>

std::unique_ptr<BufferMan> BufferMan::s_Instance;

constexpr bool Partition::operator()(const Partition& x, const Partition& y) const {
    return x.size < y.size;
}

Memory::Memory(std::uint8_t* ptr, D3D12_GPU_VIRTUAL_ADDRESS gPtr, Partition&& partition)
    : cpuPTR(ptr), gpuPTR(gPtr), partitionInPage(std::move(partition)) {}

BufferMan::BufferMan(Graphics& gfx)
    : m_gfx(gfx), m_DefaultSize(2048u) {}

void BufferMan::Allocate(std::uint64_t bufferSize) {
    CD3DX12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

    CD3DX12_RESOURCE_DESC rcDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

    Page page;
    page.size = bufferSize;

    GFX_THROW_FAILED(hr, GetDevice(m_gfx)->CreateCommittedResource(
        &heapProp,
        D3D12_HEAP_FLAG_NONE,
        &rcDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        __uuidof(ID3D12Resource),
        &page.memory
    ));

    GFX_THROW_FAILED(
        hr, page.memory->Map(0, nullptr, reinterpret_cast<void**>(&page.cpuPTR))
    );

    m_Memories.push_back(page);
}

std::unique_ptr<Memory> BufferMan::_RequestMemory(
    std::uint64_t bufferSize, std::uint64_t alignment
) noexcept {

    bufferSize = AlignUp(bufferSize, alignment);

    for (auto page = m_Memories.begin(); page != m_Memories.end(); ++page) {
        auto firstFind =
            std::lower_bound(
                page->m_FreePartitions.begin(), page->m_FreePartitions.end(), bufferSize,
                [&](const Partition& a, const std::uint64_t b) {
                    std::uint64_t alignedOffset = AlignUp(a.offsetFromBase, alignment);
                    std::uint64_t sizeDec = alignedOffset - a.offsetFromBase;

                    return (a.size - sizeDec) < b;
                }
        );

        if (firstFind != page->m_FreePartitions.end()) {
            Partition partition = *firstFind;
            page->m_FreePartitions.erase(firstFind);

            std::uint64_t alignedOffset = AlignUp(partition.offsetFromBase, alignment);
            std::uint64_t sizeDec = alignedOffset - partition.offsetFromBase;

            if (sizeDec) {
                Partition remains;
                remains.offsetFromBase = partition.offsetFromBase;
                remains.pageIndex = static_cast<std::uint32_t>(page - m_Memories.begin());
                remains.size = sizeDec;

                page->m_FreePartitions.insert(std::move(remains));
            }

            partition.offsetFromBase = alignedOffset;

            if ((partition.size -sizeDec) > bufferSize) {
                Partition fpartition;
                fpartition.offsetFromBase = partition.offsetFromBase + bufferSize;
                fpartition.size = partition.size - bufferSize - sizeDec;
                fpartition.pageIndex = static_cast<std::uint32_t>(page - m_Memories.begin());

                partition.size = bufferSize - sizeDec;

                page->m_FreePartitions.insert(std::move(fpartition));
            }

            return
                std::make_unique<Memory>(
                    page->cpuPTR + partition.offsetFromBase,
                    page->memory->GetGPUVirtualAddress() + partition.offsetFromBase,
                    std::move(partition)
                    );
        }
    }

    Allocate(bufferSize > m_DefaultSize ? bufferSize : m_DefaultSize);

    Partition oPartition;
    oPartition.offsetFromBase = 0u;
    oPartition.pageIndex = static_cast<std::uint32_t>(m_Memories.size()) - 1u;
    oPartition.size = bufferSize;

    if (m_Memories.back().size > bufferSize) {
        Partition fPartition;
        fPartition.offsetFromBase = bufferSize;
        fPartition.pageIndex = static_cast<std::uint32_t>(m_Memories.size()) - 1u;
        fPartition.size = m_Memories.back().size - bufferSize;

        m_Memories.back().m_FreePartitions.insert(std::move(fPartition));
    }

    return std::make_unique<Memory>(
        m_Memories.back().cpuPTR,
        m_Memories.back().memory->GetGPUVirtualAddress(),
        std::move(oPartition)
    );
}

void BufferMan::Init(Graphics& gfx) noexcept {
    s_Instance = std::make_unique<BufferMan>(gfx);
}

std::unique_ptr<Memory> BufferMan::RequestMemory(
    std::uint64_t bufferSize, std::uint64_t alignment
) noexcept {
    return s_Instance->_RequestMemory(bufferSize, alignment);
}

std::uint64_t BufferMan::AlignUp(std::uint64_t number, std::uint64_t alignment) noexcept {
    return (number + --alignment) & ~alignment;
}

void BufferMan::Free(Partition&& partition) noexcept {
    m_Memories[partition.pageIndex].m_FreePartitions.insert(std::move(partition));
}

void BufferMan::FreePartition(Partition&& partition) noexcept {
    s_Instance->Free(std::move(partition));
}
