#include <VertexBuffer.hpp>
#include <Graphics.hpp>

VertexBuffer::VertexBuffer(const Vertices& vertices)
	: m_VertexBufferView{} {

	m_pBuffer = BufferMan::RequestMemory(vertices.GetSizeByte());
	memcpy(m_pBuffer->cpuPTR, vertices.GetBuffer(), vertices.GetSizeByte());

	m_VertexBufferView.BufferLocation = m_pBuffer->gpuPTR;
	m_VertexBufferView.SizeInBytes = vertices.GetSizeByte();
	m_VertexBufferView.StrideInBytes = vertices.GetVertexSizeByte();
}

void VertexBuffer::BindCommand(Graphics& gfx) noexcept {
	GetCommandList(gfx)->IASetVertexBuffers(0u, 1u, &m_VertexBufferView);
}
