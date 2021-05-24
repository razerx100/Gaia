#ifndef __TRANSFORM_HPP__
#define __TRANSFORM_HPP__
#include <DirectXMath.h>

struct TransformData {
	DirectX::XMMATRIX model;
	DirectX::XMMATRIX view;
};

class Transform {
public:
	Transform() = default;

	DirectX::XMMATRIX GetTransform() const noexcept;

	DirectX::XMMATRIX GetTransformCM() const noexcept;

	DirectX::XMMATRIX GetTransformWithProjectionCM() const noexcept;

	TransformData GetTransformDataCM() const noexcept;

	Transform& operator=(const DirectX::XMMATRIX& matrix) noexcept;

private:
	DirectX::XMMATRIX m_Transform;
};
#endif
