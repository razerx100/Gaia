#include <ConstantBuffer.hpp>
#include <Camera.hpp>

void VertexConstantBuffer::BindCommand(Graphics& gfx) noexcept {

	DirectX::XMMATRIX mat =
		DirectX::XMMatrixTranspose(
			m_ConstantInfo.getter() *
			Camera::GetCamera() *
			Camera::GetProjection()
		);

	GetCommandList(gfx)->SetGraphicsRoot32BitConstants(
		m_RSIndex,
		m_ConstantInfo.elements,
		&mat, 0
	);
}
