#ifndef __DXGI_INFO_MANAGER_HPP__
#define __DXGI_INFO_MANAGER_HPP__
#include <CleanWin.hpp>
#include <vector>
#include <string>

class DxgiInfoManager {
public:
	DxgiInfoManager();
	~DxgiInfoManager();
	DxgiInfoManager(const DxgiInfoManager&) = delete;
	DxgiInfoManager& operator=(const DxgiInfoManager&) = delete;

	void Set() noexcept;
	std::vector<std::string> GetMessages() const;

private:
	std::uint64_t m_next;
	struct IDXGIInfoQueue* m_pDxgiInfoQueue;
};
#endif