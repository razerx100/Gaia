#ifndef __VERTEX_BUFFER_HPP__
#define __VERTEX_BUFFER_HPP__
#include <Bindable.hpp>
#include <vector>
#include <GraphicsThrowMacros.hpp>
#include <DirectXMath.h>

class VertexBuffer : public Bindable {
public:
	VertexBuffer(Graphics& gfx, const std::vector<DirectX::XMFLOAT3>& vertices);

	void Bind(Graphics& gfx) noexcept override;

protected:
	ComPtr<ID3D11Buffer> m_pVertexBuffer;

	const std::uint32_t m_Stride;
	const std::uint32_t m_Offset;
};
#endif
