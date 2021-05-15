#ifndef __COMMANDLIST_HPP__
#define __COMMANDLIST_HPP__
#include <CleanWin.hpp>
#include <d3d12.h>
#include <wrl.h>
#include <vector>
#include <queue>
#include <GraphicsExtractor.hpp>

using Microsoft::WRL::ComPtr;

class CommandList : public GraphicsExtractor {
public:
	CommandList();

	ID3D12GraphicsCommandList* Get() const noexcept;

	void Create(Graphics& gfx, D3D12_COMMAND_LIST_TYPE type, std::uint32_t bufferCount);
	void Reset();
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
#endif
