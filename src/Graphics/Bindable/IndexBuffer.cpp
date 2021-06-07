#include <IndexBuffer.hpp>
#include <Graphics.hpp>

IndexBuffer::IndexBuffer(const std::vector<std::uint16_t>& indices)
	: m_IndexBufferView{}, m_IndexCount(static_cast<std::uint32_t>(std::size(indices))) {

	const std::uint32_t indicesSize = static_cast<std::uint32_t>(
			std::size(indices) * sizeof(std::uint16_t)
		);

	m_pBuffer = BufferMan::RequestMemory(indicesSize);
	memcpy(m_pBuffer->cpuPTR, indices.data(), indicesSize);

	m_IndexBufferView.BufferLocation = m_pBuffer->gpuPTR;
	m_IndexBufferView.Format = DXGI_FORMAT_R16_UINT;
	m_IndexBufferView.SizeInBytes = indicesSize;
}

std::uint32_t IndexBuffer::GetIndexCount() const noexcept {
	return m_IndexCount;
}

void IndexBuffer::BindCommand(Graphics& gfx) noexcept {
	GetCommandList(gfx)->IASetIndexBuffer(&m_IndexBufferView);
}
