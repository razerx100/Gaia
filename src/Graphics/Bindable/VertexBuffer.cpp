#include <VertexBuffer.hpp>


VertexBuffer::VertexBuffer(Graphics& gfx, const std::vector<DirectX::XMFLOAT3>& vertices)
	: m_Stride(sizeof(DirectX::XMFLOAT3)), m_Offset(0u) {

	D3D11_BUFFER_DESC vertexDesc = {};
	vertexDesc.ByteWidth = static_cast<std::uint32_t>(m_Stride * std::size(vertices));
	vertexDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexDesc.CPUAccessFlags = 0u;
	vertexDesc.MiscFlags = 0u;
	vertexDesc.StructureByteStride = m_Stride;

	D3D11_SUBRESOURCE_DATA vertexData = {};
	vertexData.pSysMem = vertices.data();


	GFX_THROW_FAILED(hr, GetDevice(gfx)->CreateBuffer(&vertexDesc, &vertexData, &m_pVertexBuffer));
}

void VertexBuffer::Bind(Graphics& gfx) noexcept {
	GetDeviceContext(gfx)->IASetVertexBuffers(0u, 1u, m_pVertexBuffer.GetAddressOf(), &m_Stride, &m_Offset);
}
