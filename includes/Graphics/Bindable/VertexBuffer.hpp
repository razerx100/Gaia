#ifndef __VERTEX_BUFFER_HPP__
#define __VERTEX_BUFFER_HPP__
#include <Bindable.hpp>
#include <vector>
#include <DirectXMath.h>

class VertexBuffer : public BindableResource {
public:
	VertexBuffer(Graphics& gfx, std::vector<DirectX::XMFLOAT3>&& vertices);
	VertexBuffer(Graphics& gfx,
		std::vector<DirectX::XMFLOAT3>&& vertices,
		std::vector<DirectX::XMFLOAT4>&& colors
	);
	VertexBuffer(Graphics& gfx,
		std::vector<DirectX::XMFLOAT3>&& vertices,
		std::vector<DirectX::XMFLOAT3>&& normals
	);
	VertexBuffer(Graphics& gfx,
		std::vector<DirectX::XMFLOAT3>&& vertices,
		std::vector<DirectX::XMFLOAT2>&& uvs
	);

	void BindCommand(Graphics& gfx) noexcept override;

private:
	ComPtr<ID3D12Resource> m_pVertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView;
};

#endif
