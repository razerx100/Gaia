#ifndef __VERTEX_BUFFER_HPP__
#define __VERTEX_BUFFER_HPP__
#include <Bindable.hpp>
#include <vector>
#include <DirectXMath.h>

class VertexBuffer : public BindableResource {
public:
	VertexBuffer(Graphics& gfx, std::vector<DirectX::XMFLOAT3>&& vertices);
	void BindCommand(Graphics& gfx) noexcept override;

private:
	ComPtr<ID3D12Resource> m_pVertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView;
};

#endif
