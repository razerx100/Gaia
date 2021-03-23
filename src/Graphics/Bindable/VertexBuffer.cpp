#include <VertexBuffer.hpp>

void VertexBuffer::Bind(Graphics& gfx) noexcept {
	GetDeviceContext(gfx)->IASetVertexBuffers(0u, 1u, m_pVertexBuffer.GetAddressOf(), &m_Stride, &m_Offset);
}
