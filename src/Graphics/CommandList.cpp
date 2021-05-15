#include <CommandList.hpp>
#include <Graphics.hpp>
#include <GraphicsThrowMacros.hpp>

CommandList::CommandList() {}

void CommandList::Create(
	Graphics& gfx, D3D12_COMMAND_LIST_TYPE type, std::uint32_t bufferCount
) {

	for (std::uint32_t index = 0; index < bufferCount; index++) {
		m_pCommandAllocators.emplace_back();
		m_FreeAllocators.push(index);

		GFX_THROW_FAILED(hr,
			GetDevice(gfx)->CreateCommandAllocator(
				type,
				__uuidof(ID3D12CommandAllocator),
				&m_pCommandAllocators.back()
			));
	}

	GFX_THROW_FAILED(hr,
		GetDevice(gfx)->CreateCommandList(
			0, type,
			m_pCommandAllocators[0].Get(),
			nullptr,
			__uuidof(ID3D12GraphicsCommandList),
			&m_pCommandList
		)
	);
}

ID3D12GraphicsCommandList* CommandList::Get() const noexcept {
	return m_pCommandList.Get();
}

void CommandList::Reset() {
	std::uint32_t currentIndex = m_FreeAllocators.front();

	GFX_THROW_FAILED(hr, m_pCommandAllocators[currentIndex]->Reset());

	GFX_THROW_FAILED(hr,
		m_pCommandList->Reset(m_pCommandAllocators[currentIndex].Get(), nullptr)
	);
}

void CommandList::Close() {
	GFX_THROW_FAILED(hr, m_pCommandList->Close());

	std::uint32_t currentIndex = m_FreeAllocators.front();

	m_InUsedAllocators.push(currentIndex);

	m_FreeAllocators.pop();
}

void CommandList::FinishedExecution() noexcept {
	std::uint32_t finishedIndex = m_InUsedAllocators.front();

	m_FreeAllocators.push(finishedIndex);

	m_InUsedAllocators.pop();
}

ID3D12GraphicsCommandList* CommandList::operator->() const noexcept {
	return Get();
}
