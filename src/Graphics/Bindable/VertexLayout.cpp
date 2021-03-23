#include <VertexLayout.hpp>
#include <GraphicsThrowMacros.hpp>

VertexLayout::VertexLayout(
	Graphics& gfx, const std::vector<D3D11_INPUT_ELEMENT_DESC>& inputDescs,
	ID3DBlob* vertexShaderByteCode
) {

	GFX_THROW_FAILED(hr, GetDevice(gfx)->CreateInputLayout(
		inputDescs.data(), static_cast<std::uint32_t>(std::size(inputDescs)),
		vertexShaderByteCode->GetBufferPointer(),
		vertexShaderByteCode->GetBufferSize(),
		&m_pInputLayout
	));
}

void VertexLayout::Bind(Graphics& gfx) noexcept {
	GetDeviceContext(gfx)->IASetInputLayout(m_pInputLayout.Get());
}
