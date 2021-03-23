#include <Drawable.hpp>

Drawable::Drawable() noexcept
	: m_IndexCount(0u), m_VertexCBuffer(nullptr){
	SetShaderPath();
}

void Drawable::Draw(Graphics& gfx) const noexcept(!IS_DEBUG) {
	for (auto& bind : m_Binds)
		bind->Bind(gfx);
	gfx.DrawIndexed(m_IndexCount);
}

void Drawable::AddBind(std::unique_ptr<Bindable> bind) noexcept {
	m_Binds.emplace_back(std::move(bind));
}

void Drawable::AddIndexBuffer(std::unique_ptr<IndexBuffer> indexBuffer) noexcept {
	m_IndexCount = indexBuffer->GetIndexCount();
	m_Binds.emplace_back(std::move(indexBuffer));
}

void Drawable::AddCVertexBuffer(std::unique_ptr<Mutable> vCBuffer) noexcept {
	m_VertexCBuffer = vCBuffer.get();
	m_Binds.emplace_back(std::move(vCBuffer));
}

void Drawable::SetShaderPath() noexcept {
	wchar_t path[MAX_PATH];
	GetModuleFileNameW(nullptr, path, MAX_PATH);
	m_ShaderPath = path;
	for (int i = static_cast<int>(m_ShaderPath.size() - 1); m_ShaderPath[i] != L'\\'; i--)
		m_ShaderPath.pop_back();

	m_ShaderPath.append(L"shaders\\");
}
