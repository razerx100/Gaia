#ifndef __VERTEX_BUFFER_HPP__
#define __VERTEX_BUFFER_HPP__
#include <Bindable.hpp>
#include <vector>
#include <DirectXMath.h>
#include <BufferMan.hpp>

class VertexBuffer : public Bindable {
public:
	VertexBuffer(std::vector<DirectX::XMFLOAT3>&& vertices);
	VertexBuffer(
		std::vector<DirectX::XMFLOAT3>&& vertices,
		std::vector<DirectX::XMFLOAT4>&& colors
	);
	VertexBuffer(
		std::vector<DirectX::XMFLOAT3>&& vertices,
		std::vector<DirectX::XMFLOAT3>&& normals
	);
	VertexBuffer(
		std::vector<DirectX::XMFLOAT3>&& vertices,
		std::vector<DirectX::XMFLOAT2>&& uvs
	);

	void BindCommand(Graphics& gfx) noexcept override;

private:
	std::unique_ptr<Memory> m_pBuffer;
	D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView;
};

#endif
