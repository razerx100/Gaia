#ifndef __VERTEX_BUFFER_HPP__
#define __VERTEX_BUFFER_HPP__
#include <Bindable.hpp>
#include <vector>
#include <DirectXMath.h>
#include <BufferMan.hpp>
#include <Vertices.hpp>

class VertexBuffer : public Bindable {
public:
	VertexBuffer(const Vertices& vertices);

	void BindCommand(Graphics& gfx) noexcept override;

private:
	std::unique_ptr<Memory> m_pBuffer;
	D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView;
};

#endif
