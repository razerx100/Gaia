#include <BindableCodex.hpp>
#include <Bindable.hpp>

BindPtr::BindPtr(const std::string& key, std::unique_ptr<Bindable> bindPtr) noexcept
	: m_pBind(std::move(bindPtr)), m_refCount(0u), m_keyName(key) {}

BindPtr::BindPtr(BindPtr&& bindPtr) noexcept
	: m_pBind(std::move(bindPtr.m_pBind)),
	m_refCount(bindPtr.m_refCount), m_keyName(bindPtr.m_keyName) {}

BindPtr& BindPtr::operator++() noexcept {
	++m_refCount;
	return *this;
}

BindPtr& BindPtr::operator--() noexcept {
	--m_refCount;
	return *this;
}

BindPtr& BindPtr::operator=(BindPtr&& bindPtr) noexcept {
	m_pBind = std::move(bindPtr.m_pBind);
	m_refCount = bindPtr.m_refCount;
	m_keyName = bindPtr.m_keyName;

	return *this;
}

// Codex

BindPtr* Codex::GetBindableRef(const std::string& key) noexcept {
	auto result = s_Instance.m_codex.find(key);

	if (result == s_Instance.m_codex.end()) [[unlikely]]
		return nullptr;
	else [[likely]] {
		++result->second;
		return &result->second;
	}
}

bool Codex::IsInCodex(const std::string& key) noexcept {
	return s_Instance.m_codex.contains(key);
}

BindPtr* Codex::AddAndGetBind(
	const std::string& key, std::unique_ptr<Bindable> bind
) noexcept {

	auto result = s_Instance.m_codex.emplace(
		std::make_pair(key, BindPtr(key, std::move(bind)))
	);

	++result.first->second;
	return &result.first->second;
}

void Codex::ReleaseRef(const std::string& key) noexcept {
	auto result = s_Instance.m_codex.find(key);

	if (result != s_Instance.m_codex.end()) {
		--(result->second);

		if (!result->second.m_refCount)
			s_Instance.m_codex.erase(result);
	}
}
