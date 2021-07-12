#include <SolidSphere.hpp>
#include <Graphics.hpp>
#include <Sphere.hpp>
#include <BindAll.hpp>
#include <App.hpp>
#include <BindableCodex.hpp>
#include <PSODesc.hpp>
#include <BindableProcessor.hpp>

SolidSphere::SolidSphere(Graphics& gfx, float radius) {
	BindProcessor process = BindProcessor(
		"SolidSphereWhite",
		"SolidColor",
		false
	);

	IndexedTriangleList model = Sphere::Make();

	model.Transform(DirectX::XMMatrixScaling(radius, radius, radius));

	process.ProcessWithoutTex(gfx, model);

	AddBind(process.GetPipelineState());
	AddBind(process.GetRootSignature());
	AddBind(process.GetTopology());
	AddBind(process.GetVertexBuffer());
	AddBind(process.GetIndexBuffer());

	struct ConstantBufferColor {
		struct {
			float red;
			float green;
			float blue;
			float alpha;
		}fill_color;
	};

	ConstantBufferColor materialColor = {
		{1.0f, 1.0f, 1.0f, 1.0f}
	};

	AddBind(BindProcessor::GetOrAddGeneric
		<ConstantBufferCBVStatic<ConstantBufferColor>>(
			"CBVStaticWhite",
			1u, &materialColor
			)
	);

	AddBind(
		std::make_unique<ConstantBufferMat>(
			0u, 16u, std::bind(&Transform::GetTransformWithProjectionCM, &m_transform)
			)
	);

	m_transform = DirectX::XMMatrixTranslation(1.0f, 1.0f, 1.0f);
}

void SolidSphere::SetPosition(DirectX::XMFLOAT3 position) noexcept {
	m_transform = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
}
