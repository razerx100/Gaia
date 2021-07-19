#ifndef __COMMAND_QUEUE_HPP__
#define __COMMAND_QUEUE_HPP__
#include <CleanWin.hpp>
#include <d3d12.h>
#include <wrl.h>
#include <vector>
#include <memory>
#include <queue>
#include <GraphicsExtractor.hpp>

using Microsoft::WRL::ComPtr;

class CommandList : public GraphicsExtractor {
public:
	CommandList() = default;
	CommandList(
		Graphics& gfx, D3D12_COMMAND_LIST_TYPE type,
		std::uint32_t allocatorsPerCmdList
	);

	ID3D12GraphicsCommandList* Get() const noexcept;

	void Init(
		Graphics& gfx, D3D12_COMMAND_LIST_TYPE type,
		std::uint32_t allocatorsPerCmdList
	);
	void Record();
	void Close();
	void FinishedExecution() noexcept;

	ID3D12GraphicsCommandList* operator->() const noexcept;

private:
	ComPtr<ID3D12GraphicsCommandList> m_pCommandList;
	std::vector<ComPtr<ID3D12CommandAllocator>> m_pCommandAllocators;
	std::queue<std::uint32_t> m_FreeAllocators;
	std::queue<std::uint32_t> m_InUsedAllocators;

	HRESULT hr;
};

class CommandQueue : public GraphicsExtractor {
public:
	void Init(
		Graphics& gfx,
		D3D12_COMMAND_LIST_TYPE type,
		std::uint32_t allocatorsPerCmdList,
		std::uint32_t numberOfCommandLists = 1u
	);
	void ExecuteCommandLists();
	void SubmitCommandList(CommandList* commandList) noexcept;

	CommandList* GetCommandList(std::uint32_t index) const noexcept;
	ID3D12CommandQueue* GetCommandQueue() const noexcept;
	D3D12_COMMAND_LIST_TYPE GetQueueType() const noexcept;

private:
    ComPtr<ID3D12CommandQueue> m_pCommandQueue;
	std::vector<std::unique_ptr<CommandList>> m_commandLists;
	std::queue<ID3D12CommandList*> m_recordedLists;
	D3D12_COMMAND_LIST_TYPE m_commandQueueType;

	HRESULT hr;
};
#endif
