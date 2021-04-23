#ifndef __INDEX_BUFFER_HPP__
#define __INDEX_BUFFER_HPP__
#include <Bindable.hpp>
#include <vector>
#include <BufferMan.hpp>

class IndexBuffer : public Bindable {
public:
	IndexBuffer(std::vector<std::uint16_t>&& indices);
	void BindCommand(Graphics& gfx) noexcept override;

	std::uint32_t GetIndexCount() const noexcept;

private:
	std::unique_ptr<Memory> m_pBuffer;

	D3D12_INDEX_BUFFER_VIEW m_IndexBufferView;

	std::uint32_t m_IndexCount;
};

#endif