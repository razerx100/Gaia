#include <ConstantBuffer.hpp>
#include <Camera.hpp>

void VertexConstantBuffer::BindCommand(Graphics& gfx) noexcept {
	DirectX::XMMATRIX transform = DirectX::XMMatrixTranspose(
		m_ParentRef.GetTransformationMatrix() *
		Camera::GetCamera() *
		Camera::GetProjection()
	);

	GetCommandList(gfx)->SetGraphicsRoot32BitConstants(
		m_Index,
		m_ElementsNum,
		&transform, 0
	);
}
