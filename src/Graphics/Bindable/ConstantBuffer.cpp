#include <ConstantBuffer.hpp>

void VertexConstantBuffer::BindCommand(Graphics& gfx) noexcept {
	DirectX::XMMATRIX transform = DirectX::XMMatrixTranspose(
		m_ParentRef.GetTransformationMatrix()
	);

	GetCommandList(gfx)->SetGraphicsRoot32BitConstants(
		m_Index,
		m_ElementsNum,
		&transform, 0
	);
}
