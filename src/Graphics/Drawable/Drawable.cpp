#include <Drawable.hpp>
#include <Bindable.hpp>
#include <IndexBuffer.hpp>

void Drawable::Draw(Graphics& gfx) const noexcept(!IS_DEBUG) {
	for (auto& bind : m_Binds)
		bind->BindCommand(gfx);

	gfx.DrawIndexed(GetIndexCount());
}

void Drawable::Update(float deltaTime) noexcept {}

void Drawable::AddBind(std::unique_ptr<Bindable> bind) noexcept {
	m_Binds.emplace_back(std::move(bind));
}

std::uint32_t Drawable::GetIndexCount() const noexcept {
	return m_IndexCount;
}

void Drawable::AddIndexBuffer(std::unique_ptr<IndexBuffer> indexBuffer) noexcept {
	m_IndexCount = indexBuffer->GetIndexCount();
	m_Binds.emplace_back(std::move(indexBuffer));
}
