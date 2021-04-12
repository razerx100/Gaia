#include <Camera.hpp>

DirectX::XMMATRIX Camera::s_Camera;
DirectX::XMMATRIX Camera::s_Projection;

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
