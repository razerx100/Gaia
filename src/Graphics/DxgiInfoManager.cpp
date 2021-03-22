#include <DxgiInfoManager.hpp>
#include <memory>
#include <functional>
#include <WindowThrowMacros.hpp>
#include <GraphicsThrowMacros.hpp>

DxgiInfoManager::DxgiInfoManager()
	: m_next(0u), m_pDxgiInfoQueue(nullptr) {
	//typedef HRESULT(WINAPI* DXGIGetDebugInterface)(REFIID, void**); // C Style function pointer

	const HMODULE hModDxgiDebug = LoadLibraryEx("dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
	if (!hModDxgiDebug)
		throw HWND_LAST_EXCEPT();

	std::function<HRESULT(REFIID, void**)> DxgiGetDebugInterface = reinterpret_cast<HRESULT(__stdcall *)(REFIID, void**)>(
		GetProcAddress(hModDxgiDebug, "DXGIGetDebugInterface")
		);
	if (!DxgiGetDebugInterface)
		throw HWND_LAST_EXCEPT();

	HRESULT hr;
	GFX_THROW_NOINFO(hr, DxgiGetDebugInterface(__uuidof(IDXGIInfoQueue), &m_pDxgiInfoQueue));

}

void DxgiInfoManager::Set() noexcept {
	// Set the index (next) so that the next call to GetMessages()
	// will only get errors generated after this call
	m_next = m_pDxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
}

std::vector<std::string> DxgiInfoManager::GetMessages() const {
	std::vector<std::string> messages;
	const std::uint64_t end = m_pDxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);

	for (std::uint64_t i = m_next; i < end; i++) {
		HRESULT hr;
		SIZE_T messageLength = 0;
		GFX_THROW_NOINFO(hr, m_pDxgiInfoQueue->GetMessage(
			DXGI_DEBUG_ALL, i, nullptr, &messageLength
		));

		std::unique_ptr<byte[]> bytes = std::make_unique<byte[]>(messageLength);
		DXGI_INFO_QUEUE_MESSAGE* pMessage = reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE*>(bytes.get());

		GFX_THROW_NOINFO(hr, m_pDxgiInfoQueue->GetMessage(
			DXGI_DEBUG_ALL, i, pMessage, &messageLength
		));
		messages.emplace_back(pMessage->pDescription);
	}

	return messages;
}


#ifdef _DEBUG

DxgiInfoManager DxgiInfoManager::s_InfoManager;

DxgiInfoManager& DxgiInfoManager::GetDXGIInfoManager() noexcept {
	return s_InfoManager;
}

#endif
