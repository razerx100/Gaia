#include <Drawable.hpp>

std::wstring Drawable::s_ShaderPath;

void Drawable::Draw(Graphics& gfx) const noexcept(!IS_DEBUG) {
	for (auto& bind : GetStaticBindables())
		bind->BindCommand(gfx);

	for (auto& bind : m_Binds)
		bind->BindCommand(gfx);

	gfx.DrawIndexed(GetIndexCount());
}

void Drawable::SetShaderPath() noexcept {
	wchar_t path[MAX_PATH];
	GetModuleFileNameW(nullptr, path, MAX_PATH);
	s_ShaderPath = path;
	for (int i = static_cast<int>(s_ShaderPath.size() - 1); s_ShaderPath[i] != L'\\'; i--)
		s_ShaderPath.pop_back();

	s_ShaderPath.append(L"shaders\\");
}

// Bind Root Signature before constant buffers
void Drawable::AddBind(std::unique_ptr<Bindable> bind) noexcept {
	m_Binds.emplace_back(std::move(bind));
}

void Drawable::AddIndexBuffer(std::unique_ptr<IndexBuffer> indexBuffer) noexcept {
	m_IndexCount = indexBuffer->GetIndexCount();
	m_Binds.emplace_back(std::move(indexBuffer));
}
