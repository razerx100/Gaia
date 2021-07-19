#include <DebugInfoManager.hpp>
#include <WindowThrowMacros.hpp>
#include <GraphicsThrowMacros.hpp>

DebugInfoManager::DebugInfoManager()
	: m_next(0u), m_pInfoQueue(nullptr) {}

void DebugInfoManager::Set() noexcept {
	// Set the index (next) so that the next call to GetMessages()
	// will only get errors generated after this call
	m_next = m_pInfoQueue->GetNumStoredMessages();
}

std::vector<std::string> DebugInfoManager::GetMessages() const {
	std::vector<std::string> messages;
	const std::uint64_t end = m_pInfoQueue->GetNumStoredMessages();

	for (std::uint64_t i = m_next; i < end; i++) {
		HRESULT hr;
		SIZE_T messageLength = 0;
		GFX_THROW_NOINFO(hr, m_pInfoQueue->GetMessageA(
			i, nullptr, &messageLength
		));

		D3D12_MESSAGE* pMessage = new D3D12_MESSAGE[messageLength];

		GFX_THROW_NOINFO(hr, m_pInfoQueue->GetMessage(
			i, pMessage, &messageLength
		));
		messages.emplace_back(pMessage->pDescription);
	}

	return messages;
}

#ifdef _DEBUG

DebugInfoManager DebugInfoManager::s_InfoManager;

void DebugInfoManager::SetDebugInfoManager(ID3D12Device5* device) noexcept {
    device->QueryInterface(__uuidof(ID3D12InfoQueue), &s_InfoManager.m_pInfoQueue);
}

DebugInfoManager& DebugInfoManager::GetDebugInfoManager() noexcept {
	return s_InfoManager;
}
#endif
