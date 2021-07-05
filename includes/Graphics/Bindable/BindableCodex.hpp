#ifndef __BINDABLE_CODEX_HPP__
#define __BINDABLE_CODEX_HPP__
#include <unordered_map>
#include <memory>

class Bindable;

class BindPtr {
public:
	BindPtr(std::unique_ptr<Bindable> bindPtr);

	BindPtr& operator=(std::unique_ptr<Bindable> bindPtr) noexcept;
	BindPtr& operator++() noexcept;
	BindPtr& operator--() noexcept;

	std::uint32_t GetRefCount() const noexcept;
	Bindable* GetPtr() const noexcept;

private:
	std::unique_ptr<Bindable> m_pBind;
	std::uint32_t m_refCount;
};

class Codex {
public:
	static Bindable* GetBindableRef(std::string key) noexcept;
	static void AddBind(std::string key, std::unique_ptr<Bindable> bind) noexcept;
	static void ReleaseRef(std::string key) noexcept;

private:
	std::unordered_map<std::string, BindPtr> m_codex;

	static Codex s_Instance;
};

#endif
