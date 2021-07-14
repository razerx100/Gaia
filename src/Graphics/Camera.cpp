#include <Camera.hpp>
#include <ImGuiImpl.hpp>

Camera* Camera::s_pCamera;
DirectX::XMMATRIX Camera::s_projection;

Camera::Camera()
	: m_position{0.0f, 0.0f, -20.0f}, m_pitch(0.0f), m_yaw(0.0f) {}

void Camera::SetCameraInstance(Camera* camera) noexcept {
	s_pCamera = camera;
}

void Camera::SetProjection(DirectX::XMMATRIX projection) noexcept {
	s_projection = projection;
}

DirectX::XMMATRIX Camera::GetCameraMatrix() noexcept {
	return s_pCamera->GetMatrix();
}

DirectX::XMFLOAT3 Camera::GetCameraPos() noexcept {
	return s_pCamera->m_position;
}

DirectX::XMMATRIX Camera::GetProjection() noexcept {
	return s_projection;
}

DirectX::XMMATRIX Camera::GetMatrix() const noexcept {
	DirectX::XMVECTOR forwardBaseVector = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	DirectX::XMVECTOR lookVector = DirectX::XMVector3Transform(
		forwardBaseVector,
		DirectX::XMMatrixRotationRollPitchYaw(m_pitch, m_yaw, 0.0f)
	);

	DirectX::XMVECTOR cameraPosition = DirectX::XMLoadFloat3(&m_position);

	return DirectX::XMMatrixLookAtLH(
		cameraPosition,
		DirectX::XMVectorAdd(cameraPosition, lookVector),
		DirectX::XMVectorSet(
			0.0f, 1.0f, 0.0f, 0.0f
		)
	);
}

void Camera::Reset() noexcept {
	m_position = { 0.0f, 0.0f, -20.0f };

	m_pitch = 0.0f;
	m_yaw = 0.0f;
}

void Camera::ControlWindow() noexcept {
	ImGuiImpl::ImGuiRenderCameraControl(
		m_position, m_pitch, m_yaw, std::bind(&Camera::Reset, this)
	);
}

void Camera::Rotate(float dx, float dy) noexcept {
	float theta = m_yaw + dx * m_rotationSpeed;
	float modded = std::fmod(theta, DirectX::XM_2PI);

	m_yaw = (modded > DirectX::XM_PI) ? (modded - DirectX::XM_2PI) : modded;

	m_pitch = std::clamp(
		m_pitch + dy * m_rotationSpeed,
		0.995f * -DirectX::XM_PIDIV2, 0.995f * DirectX::XM_PIDIV2
	);
}

void Camera::Translate(DirectX::XMFLOAT3 translation) noexcept {
	DirectX::XMStoreFloat3(
		&translation, DirectX::XMVector3Transform(
			DirectX::XMLoadFloat3(&translation),
			DirectX::XMMatrixRotationRollPitchYaw(m_pitch, m_yaw, 0.0f) *
			DirectX::XMMatrixScaling(m_travelSpeed, m_travelSpeed, m_travelSpeed)
		)
	);

	m_position = {
		m_position.x + translation.x,
		m_position.y + translation.y,
		m_position.z + translation.z
	};
}
