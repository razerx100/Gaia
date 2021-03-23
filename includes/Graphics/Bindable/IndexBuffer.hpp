#ifndef __INDEX_BUFFER_HPP__
#define __INDEX_BUFFER_HPP__
#include <Bindable.hpp>
#include <vector>

class IndexBuffer : public Bindable {
public:
	IndexBuffer(Graphics& gfx, const std::vector<std::uint16_t>& indices);

	void Bind(Graphics& gfx) noexcept override;
	std::uint32_t GetIndexCount() const noexcept;

protected:
	std::uint32_t m_IndexCount;
	ComPtr<ID3D11Buffer> m_pIndexBuffer;
};
#endif
