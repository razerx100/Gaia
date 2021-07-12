#ifndef __VERTEX_BUFFER_HPP__
#define __VERTEX_BUFFER_HPP__
#include <Bindable.hpp>
#include <DirectXMath.h>
#include <memory>

class VertexBuffer : public Bindable {
public:
	VertexBuffer(const class Vertices& vertices);

	void BindCommand(Graphics& gfx) noexcept override;

private:
	std::unique_ptr<struct Memory> m_pBuffer;
	D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView;
};

#endif
