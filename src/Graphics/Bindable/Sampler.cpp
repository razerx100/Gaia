#include <Sampler.hpp>
#include <GraphicsThrowMacros.hpp>

Sampler::Sampler(Graphics& gfx) {
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	GFX_THROW_FAILED(hr, GetDevice(gfx)->CreateSamplerState(
		&samplerDesc, &m_pSampler
	));
}

void Sampler::Bind(Graphics& gfx) noexcept {
	GetDeviceContext(gfx)->PSSetSamplers(0, 1, m_pSampler.GetAddressOf());
}
