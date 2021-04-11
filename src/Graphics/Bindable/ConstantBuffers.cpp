#include <ConstantBuffers.hpp>

void VertexConstantBuffer::Bind(Graphics& gfx) noexcept {
	DirectX::XMMATRIX transform = DirectX::XMMatrixTranspose(
		m_ParentRef.GetTransformationMatrix()
	);

	Update(gfx, transform);

	Bindable::GetDeviceContext(gfx)->VSSetConstantBuffers(
		0u, 1u, s_pConstantBuffer.GetAddressOf()
	);
}
