#include <CommandQueue.hpp>
#include <Graphics.hpp>
#include <GraphicsThrowMacros.hpp>

// Command List
CommandList::CommandList(
	Graphics& gfx, D3D12_COMMAND_LIST_TYPE type, std::uint32_t allocatorsPerCmdList
) {
	Init(gfx, type, allocatorsPerCmdList);
}
void CommandList::Init(
	Graphics& gfx, D3D12_COMMAND_LIST_TYPE type, std::uint32_t allocatorsPerCmdList
) {
	for (std::uint32_t index = 0; index < allocatorsPerCmdList; index++) {
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
		GetDevice(gfx)->CreateCommandList1(
			0, type,
			D3D12_COMMAND_LIST_FLAG_NONE,
			__uuidof(ID3D12GraphicsCommandList),
			&m_pCommandList
		)
	);
}

ID3D12GraphicsCommandList* CommandList::Get() const noexcept {
	return m_pCommandList.Get();
}

void CommandList::Record() {
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

// Command Queue

void CommandQueue::Init(
	Graphics& gfx,
	D3D12_COMMAND_LIST_TYPE type,
	std::uint32_t allocatorsPerCmdList,
	std::uint32_t numberOfCommandLists
) {
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = type;

	m_commandQueueType = type;

    GFX_THROW_FAILED(hr, GetDevice(gfx)->CreateCommandQueue(
        &queueDesc, __uuidof(ID3D12CommandQueue), &m_pCommandQueue
    ));

	for (std::uint32_t i = 0; i < numberOfCommandLists; ++i)
		m_commandLists.emplace_back(
			std::make_unique<CommandList>(gfx, type, allocatorsPerCmdList)
		);
}

void CommandQueue::ExecuteCommandLists() {
    ID3D12CommandList* ppCommandLists[] = { m_recordedLists.front() };
	m_recordedLists.pop();

	m_pCommandQueue->ExecuteCommandLists(
        static_cast<std::uint32_t>(std::size(ppCommandLists)), ppCommandLists
    );
}

void CommandQueue::SubmitCommandList(CommandList* commandList) noexcept {
	m_recordedLists.emplace(commandList->Get());
}

CommandList* CommandQueue::GetCommandList(
	std::uint32_t index
) const noexcept {
	return m_commandLists[index].get();
}

ID3D12CommandQueue* CommandQueue::GetCommandQueue() const noexcept {
	return m_pCommandQueue.Get();
}

D3D12_COMMAND_LIST_TYPE CommandQueue::GetQueueType() const noexcept {
	return m_commandQueueType;
}
