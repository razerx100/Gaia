#include <BindableCodex.hpp>
#include <Bindable.hpp>

BindPtr::BindPtr(std::unique_ptr<Bindable> bindPtr)
	: m_pBind(std::move(bindPtr)), m_refCount(0u) {}

BindPtr& BindPtr::operator=(std::unique_ptr<Bindable> bindPtr) noexcept {
	m_pBind = std::move(bindPtr);
	return *this;
}

Bindable* BindPtr::GetPtr() const noexcept {
	return m_pBind.get();
}

BindPtr& BindPtr::operator++() noexcept {
	++m_refCount;
	return *this;
}

BindPtr& BindPtr::operator--() noexcept {
	--m_refCount;
	return *this;
}

std::uint32_t BindPtr::GetRefCount() const noexcept {
	return m_refCount;
}

Codex Codex::s_Instance;

Bindable* Codex::GetBindableRef(std::string key) noexcept {
	auto result = s_Instance.m_codex.find(key);

	if (result == s_Instance.m_codex.end())
		return nullptr;
	else {
		++result->second;
		return result->second.GetPtr();
	}
}

void Codex::AddBind(std::string key, std::unique_ptr<Bindable> bind) noexcept {
	s_Instance.m_codex.emplace(std::make_pair(key, std::move(bind)));
}

void Codex::ReleaseRef(std::string key) noexcept {
	auto result = s_Instance.m_codex.find(key);
	--result->second;

	if (!result->second.GetRefCount())
		s_Instance.m_codex.erase(result);
}
