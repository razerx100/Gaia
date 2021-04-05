#ifndef __VERTEX_BUFFER_HPP__
#define __VERTEX_BUFFER_HPP__
#include <Bindable.hpp>
#include <vector>
#include <GraphicsThrowMacros.hpp>
#include <DirectXMath.h>

class VertexBuffer : public Bindable {
public:
	VertexBuffer(Graphics& gfx, std::vector<DirectX::XMFLOAT3>&& vertices);
	VertexBuffer(
			Graphics& gfx,
			std::vector<DirectX::XMFLOAT3>&& positions,
			std::vector<DirectX::XMFLOAT4>&& colors
		);
	VertexBuffer(
			Graphics& gfx,
			std::vector<DirectX::XMFLOAT3>&& positions,
			std::vector<DirectX::XMFLOAT2>&& uvs
		);

	void Bind(Graphics& gfx) noexcept override;

protected:
	ComPtr<ID3D11Buffer> m_pVertexBuffer;

	std::uint32_t m_Stride;
	std::uint32_t m_Offset;
};
#endif
