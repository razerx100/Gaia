#include <ConstantBuffers.hpp>
#include <Camera.hpp>

void VertexConstantBuffer::Bind(Graphics& gfx) noexcept {
	DirectX::XMMATRIX transform = DirectX::XMMatrixTranspose(
		m_ParentRef.GetTransformationMatrix() *
		Camera::GetCamera() *
		Camera::GetProjection()
	);

	Update(gfx, transform);

	Bindable::GetDeviceContext(gfx)->VSSetConstantBuffers(
		0u, 1u, s_pConstantBuffer.GetAddressOf()
	);
}
