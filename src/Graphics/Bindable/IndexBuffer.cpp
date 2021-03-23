#include <IndexBuffer.hpp>
#include <GraphicsThrowMacros.hpp>

IndexBuffer::IndexBuffer(Graphics& gfx, const std::vector<std::uint16_t>& indices)
	: m_IndexCount(static_cast<std::uint32_t>(std::size(indices))) {

	D3D11_BUFFER_DESC indexDesc;
	indexDesc.ByteWidth = static_cast<std::uint32_t>(m_IndexCount * sizeof(std::uint16_t));
	indexDesc.Usage = D3D11_USAGE_DEFAULT;
	indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexDesc.CPUAccessFlags = 0u;
	indexDesc.MiscFlags = 0u;
	indexDesc.StructureByteStride = sizeof(std::uint16_t);

	D3D11_SUBRESOURCE_DATA indexData = {};
	indexData.pSysMem = indices.data();

	GFX_THROW_FAILED(hr, GetDevice(gfx)->CreateBuffer(
		&indexDesc, &indexData, &m_pIndexBuffer
	));
}

void IndexBuffer::Bind(Graphics& gfx) noexcept {
	GetDeviceContext(gfx)->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);
}

std::uint32_t IndexBuffer::GetIndexCount() const noexcept {
	return m_IndexCount;
}
