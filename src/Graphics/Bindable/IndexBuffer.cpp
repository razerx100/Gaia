#include <IndexBuffer.hpp>
#include <Graphics.hpp>

IndexBuffer::IndexBuffer(Graphics& gfx, std::vector<std::uint16_t>&& indices)
	: m_IndexBufferView{}, m_IndexCount(static_cast<std::uint32_t>(std::size(indices))) {

	const std::uint32_t indicesSize = static_cast<std::uint32_t>(
			std::size(indices) * sizeof(std::uint16_t)
		);

	CreateResource(gfx, indices.data(), indicesSize, &m_pIndexBuffer);

	m_IndexBufferView.BufferLocation = m_pIndexBuffer->GetGPUVirtualAddress();
	m_IndexBufferView.Format = DXGI_FORMAT_R16_UINT;
	m_IndexBufferView.SizeInBytes = indicesSize;
}

std::uint32_t IndexBuffer::GetIndexCount() const noexcept {
	return m_IndexCount;
}

void IndexBuffer::BindCommand(Graphics& gfx) noexcept {
	GetCommandList(gfx)->IASetIndexBuffer(&m_IndexBufferView);
}
