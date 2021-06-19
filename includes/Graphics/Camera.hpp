#ifndef __CAMERA_HPP__
#define __CAMERA_HPP__
#include <DirectXMath.h>

class Camera {
public:
	Camera();
	~Camera() = default;

	static void SetCameraInstance(Camera* camera) noexcept;
	static void SetProjection(DirectX::XMMATRIX projection) noexcept;
	static DirectX::XMMATRIX GetCameraMatrix() noexcept;
	static DirectX::XMMATRIX GetProjection() noexcept;

	DirectX::XMMATRIX GetMatrix() const noexcept;

	void Reset() noexcept;
	void ControlWindow() noexcept;
	void Rotate(float dx, float dy) noexcept;
	void Translate(DirectX::XMFLOAT3 translation) noexcept;

private:
	DirectX::XMFLOAT3  m_position;
	float m_pitch;
	float m_yaw;

	static constexpr float m_travelSpeed = 12.0f;
	static constexpr float m_rotationSpeed = 0.004f;

	static Camera* s_pCamera;
	static DirectX::XMMATRIX s_projection;
};
#endif
