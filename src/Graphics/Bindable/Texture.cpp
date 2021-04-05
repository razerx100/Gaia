#include <Texture.hpp>
#include <Surface.hpp>
#include <GraphicsThrowMacros.hpp>

Texture::Texture(Graphics& gfx, const Surface& s) {
	D3D11_TEXTURE2D_DESC texDesc = {};
	texDesc.Height = s.GetHeight();
	texDesc.Width = s.GetWidth();
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	texDesc.SampleDesc.Count = 1;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	D3D11_SUBRESOURCE_DATA sRC = {};
	sRC.pSysMem = s.GetBufferPtr();
	sRC.SysMemPitch = s.GetWidth() * sizeof(Surface::Color);

	ComPtr<ID3D11Texture2D> pTexure;
	GFX_THROW_FAILED(hr, GetDevice(gfx)->CreateTexture2D(
		&texDesc, &sRC, &pTexure
	));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	GFX_THROW_FAILED(hr, GetDevice(gfx)->CreateShaderResourceView(
		pTexure.Get(), &srvDesc, &m_pTextureView
	));
}

void Texture::Bind(Graphics& gfx) noexcept {
	GetDeviceContext(gfx)->PSSetShaderResources(0, 1, m_pTextureView.GetAddressOf());
}

