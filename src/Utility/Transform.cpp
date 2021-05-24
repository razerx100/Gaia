#include <Transform.hpp>
#include <Camera.hpp>

DirectX::XMMATRIX Transform::GetTransform() const noexcept {
	return m_Transform;
}

DirectX::XMMATRIX Transform::GetTransformCM() const noexcept {
	return DirectX::XMMatrixTranspose(m_Transform);
}

DirectX::XMMATRIX Transform::GetTransformWithProjectionCM() const noexcept {
	return DirectX::XMMatrixTranspose(
		m_Transform *
		Camera::GetCamera() *
		Camera::GetProjection()
	);
}

Transform& Transform::operator=(const DirectX::XMMATRIX& matrix) noexcept {
	m_Transform = matrix;

	return *this;
}

TransformData Transform::GetTransformDataCM() const noexcept {
	return {
		DirectX::XMMatrixTranspose(m_Transform),
		DirectX::XMMatrixTranspose(Camera::GetCamera())
	};
}
