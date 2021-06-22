#include <BufferMan.hpp>
#include <d3dx12.h>
#include <GraphicsThrowMacros.hpp>
#include <Graphics.hpp>

std::unique_ptr<BufferMan> BufferMan::s_instance;

constexpr bool Partition::operator()(const Partition& x, const Partition& y) const {
    return x.size < y.size;
}

Partition::Partition(
    std::uint64_t offsetFromBase,
    std::uint64_t size,
    std::uint32_t pageIndex
) : offsetFromBase(offsetFromBase), size(size), pageIndex(pageIndex) {}

Memory::Memory(std::uint8_t* ptr, D3D12_GPU_VIRTUAL_ADDRESS gPtr, Partition&& partition)
    : cpuPTR(ptr), gpuPTR(gPtr), partitionInPage(std::move(partition)) {}

BufferMan::BufferMan(Graphics* gfx)
    : m_gfx(gfx), m_defaultSize(2_KB) {}

void BufferMan::Allocate(std::uint64_t bufferSize) {
    CD3DX12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    CD3DX12_RESOURCE_DESC rcDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

    Page page;
    page.size = bufferSize;

    GFX_THROW_FAILED(hr, GetDevice(*m_gfx)->CreateCommittedResource(
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

    m_memories.push_back(page);
}

std::uint64_t BufferMan::GetAlignedSizeDiff(
    const Partition& partition, std::uint64_t alignment
) noexcept {
    std::uint64_t alignedOffset = AlignUp(partition.offsetFromBase, alignment);

    return alignedOffset - partition.offsetFromBase;
}

std::unique_ptr<Memory> BufferMan::_RequestMemory(
    std::uint64_t bufferSize, std::uint64_t alignment
) noexcept {

    bufferSize = AlignUp(bufferSize, alignment);

    for (auto page = m_memories.begin(); page != m_memories.end(); ++page) {
        auto firstFind =
            std::lower_bound(
                page->m_freePartitions.begin(), page->m_freePartitions.end(), bufferSize,
                [&](const Partition& a, const std::uint64_t b) {
                    std::uint64_t alignedSizeDiff = GetAlignedSizeDiff(a, alignment);
                    return (a.size - alignedSizeDiff) < b;
                }
        );

        if (firstFind != page->m_freePartitions.end()) {
            Partition partition = *firstFind;
            page->m_freePartitions.erase(firstFind);

            std::uint64_t alignedSizeDiff = GetAlignedSizeDiff(partition, alignment);

            if (alignedSizeDiff) {
                Partition remainedPartition = Partition(
                    partition.offsetFromBase,
                    alignedSizeDiff,
                    static_cast<std::uint32_t>(page - m_memories.begin())
                );

                page->m_freePartitions.insert(std::move(remainedPartition));
            }

            partition.offsetFromBase = AlignUp(partition.offsetFromBase, alignment);

            if ((partition.size - alignedSizeDiff) > bufferSize) {
                Partition freePartition = Partition(
                    partition.offsetFromBase + bufferSize,
                    partition.size - bufferSize - alignedSizeDiff,
                    static_cast<std::uint32_t>(page - m_memories.begin())
                );

                partition.size = bufferSize - alignedSizeDiff;

                page->m_freePartitions.insert(std::move(freePartition));
            }

            return
                std::make_unique<Memory>(
                    page->cpuPTR + partition.offsetFromBase,
                    page->memory->GetGPUVirtualAddress() + partition.offsetFromBase,
                    std::move(partition)
                    );
        }
    }

    Allocate(bufferSize > m_defaultSize ? bufferSize : m_defaultSize);

    Partition offsetedPartition = Partition(
        0u,
        bufferSize,
        static_cast<std::uint32_t>(m_memories.size()) - 1u
    );

    if (m_memories.back().size > bufferSize) {
        Partition freePartition = Partition(
            bufferSize,
            m_memories.back().size - bufferSize,
            static_cast<std::uint32_t>(m_memories.size()) - 1u
        );

        m_memories.back().m_freePartitions.insert(std::move(freePartition));
    }

    return std::make_unique<Memory>(
        m_memories.back().cpuPTR,
        m_memories.back().memory->GetGPUVirtualAddress(),
        std::move(offsetedPartition)
        );
}

void BufferMan::Init(Graphics* gfx) noexcept {
    s_instance = std::make_unique<BufferMan>(gfx);
}

std::unique_ptr<Memory> BufferMan::RequestMemory(
    std::uint64_t bufferSize, std::uint64_t alignment
) noexcept {
    return s_instance->_RequestMemory(bufferSize, alignment);
}

std::uint64_t BufferMan::AlignUp(std::uint64_t number, std::uint64_t alignment) noexcept {
    return (number + --alignment) & ~alignment;
}

void BufferMan::Free(Partition&& partition) noexcept {
    m_memories[partition.pageIndex].m_freePartitions.insert(std::move(partition));
}

void BufferMan::FreePartition(Partition&& partition) noexcept {
    s_instance->Free(std::move(partition));
}

// Custom Literal Suffix
constexpr std::uint64_t operator"" _B(unsigned long long number) {
    return number;
}

constexpr std::uint64_t operator"" _KB(unsigned long long number) {
    return number * 1024u;
}

constexpr std::uint64_t operator"" _MB(unsigned long long number) {
    return number * 1024u * 1024u;
}

constexpr std::uint64_t operator"" _GB(unsigned long long number) {
    return number * 1024u * 1024u * 1024u;
}
