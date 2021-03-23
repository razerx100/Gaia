#include <Shaders.hpp>
#include <GraphicsThrowMacros.hpp>

ID3DBlob* Shader::GetByteCode() const noexcept {
	return m_pByteCode.Get();
}

PixelShader::PixelShader(Graphics& gfx, const std::wstring& path) {

	GFX_THROW_FAILED(hr, D3DReadFileToBlob(
		path.c_str(), &m_pByteCode));

	GFX_THROW_FAILED(hr, GetDevice(gfx)->CreatePixelShader(
		m_pByteCode->GetBufferPointer(), m_pByteCode->GetBufferSize(),
		nullptr, &m_pPixelShader
	));
}

void PixelShader::Bind(Graphics& gfx) noexcept {
	GetDeviceContext(gfx)->PSSetShader(m_pPixelShader.Get(), nullptr, 0u);
}

VertexShader::VertexShader(Graphics& gfx, const std::wstring& path) {

	GFX_THROW_FAILED(hr, D3DReadFileToBlob(
		path.c_str(), &m_pByteCode));

	GFX_THROW_FAILED(hr, GetDevice(gfx)->CreateVertexShader(
		m_pByteCode->GetBufferPointer(), m_pByteCode->GetBufferSize(),
		nullptr, &m_pVertexShader
	));
}

void VertexShader::Bind(Graphics& gfx) noexcept {
	GetDeviceContext(gfx)->VSSetShader(m_pVertexShader.Get(), nullptr, 0u);
}
