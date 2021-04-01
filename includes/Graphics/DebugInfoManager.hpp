#ifndef __DXGI_INFO_MANAGER_HPP__
#define __DXGI_INFO_MANAGER_HPP__
#include <CleanWin.hpp>
#include <wrl.h>
#include <vector>
#include <string>
#include <d3d12.h>

using Microsoft::WRL::ComPtr;

class DebugInfoManager {
public:
	DebugInfoManager();
	~DebugInfoManager() = default;
	DebugInfoManager(const DebugInfoManager&) = delete;
	DebugInfoManager& operator=(const DebugInfoManager&) = delete;

	void Set() noexcept;
	std::vector<std::string> GetMessages() const;

	ComPtr<ID3D12InfoQueue> m_pInfoQueue;
private:
	std::uint64_t m_next;

#ifdef _DEBUG
public:
	static void SetDebugInfoManager(ID3D12Device2* device) noexcept;
	static DebugInfoManager& GetDebugInfoManager() noexcept;

private:
	static DebugInfoManager s_InfoManager;
#endif
};
#endif