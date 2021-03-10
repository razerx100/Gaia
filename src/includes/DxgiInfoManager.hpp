#ifndef __DXGI_INFO_MANAGER_HPP__
#define __DXGI_INFO_MANAGER_HPP__
#include <CleanWin.hpp>
#include <wrl.h>
#include <dxgidebug.h>
#include <vector>
#include <string>

using Microsoft::WRL::ComPtr;

class DxgiInfoManager {
public:
	DxgiInfoManager();
	~DxgiInfoManager() = default;
	DxgiInfoManager(const DxgiInfoManager&) = delete;
	DxgiInfoManager& operator=(const DxgiInfoManager&) = delete;

	void Set() noexcept;
	std::vector<std::string> GetMessages() const;

private:
	std::uint64_t m_next;
	ComPtr<IDXGIInfoQueue> m_pDxgiInfoQueue;
};
#endif