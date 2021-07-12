#ifndef __BINDABLE_CODEX_HPP__
#define __BINDABLE_CODEX_HPP__
#include <unordered_map>
#include <memory>

class Bindable;

struct BindPtr {
	BindPtr() = default;
	BindPtr(const std::string& key, std::unique_ptr<Bindable> bindPtr) noexcept;
	BindPtr(BindPtr&& bindPtr) noexcept;

	BindPtr& operator=(BindPtr&& bindPtr) noexcept;
	BindPtr& operator++() noexcept;
	BindPtr& operator--() noexcept;

	std::unique_ptr<Bindable> m_pBind;
	std::uint32_t m_refCount;
	std::string m_keyName;
};

class Codex {
public:
	// Returns null if Bindable doesn't exist in codex. Use AddAndGetBind for new Bind
	static BindPtr* GetBindableRef(const std::string& key) noexcept;
	static void ReleaseRef(const std::string& key) noexcept;
	static bool IsInCodex(const std::string& key) noexcept;
	static BindPtr* AddAndGetBind(
		const std::string& key, std::unique_ptr<Bindable> bind
	) noexcept;

private:
	std::unordered_map<std::string, BindPtr> m_codex;

	static Codex s_Instance;
};

#endif
