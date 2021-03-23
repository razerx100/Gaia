#ifndef __INDEX_BUFFER_HPP__
#define __INDEX_BUFFER_HPP__
#include <Bindable.hpp>
#include <vector>

class IndexBuffer : public Bindable {
public:
	IndexBuffer(Graphics& gfx, const std::vector<std::uint16_t>& indices);

	void Bind(Graphics& gfx) noexcept override;

protected:
	ComPtr<ID3D11Buffer> m_pIndexBuffer;
};
#endif
