#include <Drawable.hpp>
#include <Bindable.hpp>
#include <IndexBuffer.hpp>
#include <typeinfo>

Drawable::~Drawable(){}

void Drawable::Draw(Graphics& gfx) const noexcept(!IS_DEBUG) {
	for (auto& bind : m_Binds)
		bind->BindCommand(gfx);

	gfx.DrawIndexed(GetIndexCount());
}

void Drawable::AddBind(std::unique_ptr<Bindable> bind) noexcept {
	if (typeid(*bind.get()).name() == typeid(IndexBuffer).name())
		m_IndexCount = reinterpret_cast<IndexBuffer*>(bind.get())->GetIndexCount();
	m_Binds.emplace_back(std::move(bind));
}

std::uint32_t Drawable::GetIndexCount() const noexcept {
	return m_IndexCount;
}

void Drawable::AddIndexBuffer(std::unique_ptr<IndexBuffer> indexBuffer) noexcept {
	m_IndexCount = indexBuffer->GetIndexCount();
	m_Binds.emplace_back(std::move(indexBuffer));
}
