#ifndef __INDEX_BUFFER_HPP__
#define __INDEX_BUFFER_HPP__
#include <Bindable.hpp>
#include <vector>

class IndexBuffer : public BindableResource {
public:
	IndexBuffer(Graphics& gfx, std::vector<std::uint16_t>&& indices);
	void BindCommand(Graphics& gfx) noexcept override;

	std::uint32_t GetIndexCount() const noexcept;

private:
	ComPtr<ID3D12Resource> m_pIndexBuffer;
	D3D12_INDEX_BUFFER_VIEW m_IndexBufferView;

	std::uint32_t m_IndexCount;
};

#endif