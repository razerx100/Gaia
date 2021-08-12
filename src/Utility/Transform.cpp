#include <Transform.hpp>
#include <Camera.hpp>

TransformMatrices Transform::GetTransforms() const noexcept {
	TransformMatrices transforms {
		DirectX::XMMatrixTranspose(m_transform),
		DirectX::XMMatrixTranspose(Camera::GetViewMatrix()),
		DirectX::XMMatrixTranspose(Camera::GetProjection())
	};

	return transforms;
}

Transform& Transform::operator=(const DirectX::XMMATRIX& matrix) noexcept {
	m_transform = matrix;

	return *this;
}
