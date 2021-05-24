#include <Camera.hpp>
#include <ImGuiImpl.hpp>

DirectX::XMMATRIX Camera::s_Camera;
DirectX::XMMATRIX Camera::s_Projection;

Camera::Camera()
	: r(20.0f), theta(0.0f), phi(0.0f), pitch(0.0f), yaw(0.0f), roll(0.0f) {}

void Camera::SetCamera(DirectX::XMMATRIX camera) noexcept {
	s_Camera = camera;
}

void Camera::SetProjection(DirectX::XMMATRIX projection) noexcept {
	s_Projection = projection;
}

DirectX::XMMATRIX Camera::GetCamera() noexcept {
	return s_Camera;
}

DirectX::XMMATRIX Camera::GetProjection() noexcept {
	return s_Projection;
}

void Camera::Reset() noexcept {
	r = 20.0f;
	theta = 0.0f;
	phi = 0.0f;
	pitch = 0.0f;
	yaw = 0.0f;
	roll = 0.0f;
}

void Camera::Update() noexcept {
	DirectX::XMVECTOR pos = DirectX::XMVector3Transform(
		DirectX::XMVectorSet(0.0f, 0.0f, -r, 0.0f),
		DirectX::XMMatrixRotationRollPitchYaw(phi, -theta, 0.0f)
	);

	s_Camera =
		DirectX::XMMatrixLookAtLH(
			pos, DirectX::XMVectorZero(),
			DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
		) *
		DirectX::XMMatrixRotationRollPitchYaw(
			pitch, -yaw, roll
		);
}

void Camera::ControlWindow() noexcept {
	ImGuiImpl::ImGuiRenderCameraControl(r, theta, phi, roll, pitch, yaw, std::bind(&Camera::Reset, this));
}
