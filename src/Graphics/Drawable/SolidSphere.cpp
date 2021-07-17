#include <SolidSphere.hpp>
#include <Graphics.hpp>
#include <Sphere.hpp>
#include <App.hpp>
#include <BindAll.hpp>
#include <BindableCodex.hpp>
#include <BindableProcessor.hpp>
#include <Light.hpp>

SolidSphere::SolidSphere(Graphics& gfx, float radius, const std::string& name)
	: Drawable(name) {
	BindProcessor process = BindProcessor(
		"SolidSphereWhite",
		LegacyType::SolidColorNoNorm
	);

	IndexedTriangleList model = Sphere::Make();

	model.Transform(DirectX::XMMatrixScaling(radius, radius, radius));

	process.ProcessLegacyType(gfx, model);

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
			2u, &materialColor
			)
	);

	AddBind(BindProcessor::GetOrAddGeneric<ConstantBufferCBVDynamic<LightData>>(
		"CBLightData",
		1u, std::bind(&Light::GetLightData, App::GetLight())
		)
	);

	AddBind(
		std::make_unique<ConstantBufferMat>(
			0u, 16u, std::bind(&Transform::GetTransformWithProjectionCM, &m_transform)
			)
	);

	m_transform = DirectX::XMMatrixTranslation(1.0f, 1.0f, 1.0f);
}

void SolidSphere::SetPosition(const DirectX::XMFLOAT3& position) noexcept {
	m_transform = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
}
