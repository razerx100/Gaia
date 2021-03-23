#ifndef __VERTEX_BUFFER_HPP__
#define __VERTEX_BUFFER_HPP__
#include <Bindable.hpp>
#include <vector>
#include <GraphicsThrowMacros.hpp>

class VertexBuffer : public Bindable {
public:
	template<typename T>
	VertexBuffer(Graphics& gfx, const std::vector<T>& vertices)
		: m_Stride(sizeof(T)), m_Offset(0u) {

		D3D11_BUFFER_DESC vertexDesc = {};
		vertexDesc.ByteWidth = static_cast<std::uint32_t>(m_Stride * std::size(vertices));
		vertexDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexDesc.CPUAccessFlags = 0u;
		vertexDesc.MiscFlags = 0u;
		vertexDesc.StructureByteStride = m_Stride;

		D3D11_SUBRESOURCE_DATA vertexData = {};
		vertexData.pSysMem = vertices.data();


		GFX_THROW_FAILED(hr, GetDevice(gfx)->CreateBuffer(&vertexDesc, &vertexData, &m_pVertexBuffer));
	}

	void Bind(Graphics& gfx) noexcept override;

protected:
	ComPtr<ID3D11Buffer> m_pVertexBuffer;

	const std::uint32_t m_Stride;
	const std::uint32_t m_Offset;
};
#endif
