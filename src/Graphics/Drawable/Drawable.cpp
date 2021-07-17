#include <Drawable.hpp>
#include <Graphics.hpp>
#include <Bindable.hpp>
#include <IndexBuffer.hpp>
#include <BindableCodex.hpp>
#include <typeinfo>

Drawable::Drawable(const std::string& name)
	: m_indexCount(0u), m_name(name) {}

Drawable::~Drawable(){
	for (auto& bind : m_pBindRefs) {
		Codex::ReleaseRef(bind->m_keyName);
	}
}

void Drawable::Draw(Graphics& gfx) const noexcept {
	for (auto& bind : m_pBindRefs)
		bind->m_pBind->BindCommand(gfx);
	for (auto& bind : m_pBinds)
		bind->BindCommand(gfx);

	gfx.DrawIndexed(m_indexCount);
}

void Drawable::AddBind(BindPtr* bind) noexcept {
	if (typeid(*(bind->m_pBind.get())).name() == typeid(IndexBuffer).name())
		m_indexCount = reinterpret_cast<IndexBuffer*>(bind->m_pBind.get())->GetIndexCount();
	m_pBindRefs.emplace_back(std::move(bind));
}

void Drawable::AddBind(std::unique_ptr<Bindable> bind) noexcept {
	m_pBinds.emplace_back(std::move(bind));
}

const std::string& Drawable::GetName() const noexcept {
	return m_name;
}
