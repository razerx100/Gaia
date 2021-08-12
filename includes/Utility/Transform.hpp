#ifndef __TRANSFORM_HPP__
#define __TRANSFORM_HPP__
#include <DirectXMath.h>

struct TransformMatrices {
	DirectX::XMMATRIX model;
	DirectX::XMMATRIX view;
	DirectX::XMMATRIX projection;
};

class Transform {
public:
	Transform() = default;

	TransformMatrices GetTransforms() const noexcept;

	Transform& operator=(const DirectX::XMMATRIX& matrix) noexcept;

private:
	DirectX::XMMATRIX m_transform;
};
#endif
