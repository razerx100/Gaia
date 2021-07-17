#include <Quad.hpp>
#include <Graphics.hpp>
#include <BindableProcessor.hpp>
#include <Utilities.hpp>
#include <BindAll.hpp>
#include <Plane.hpp>

Quad::Quad(
	Graphics& gfx,
	const std::string& objectName,
	const std::string& texturePath
)
	: Drawable(objectName) {

	BindProcessor process;
	IndexedTriangleList model;

	if (texturePath == "") {
		process.Init(
			objectName,
			LegacyType::SolidColor
		);

		model = Plane::MakeTesselated(1, 1);
		process.ProcessLegacyType(gfx, model);
	}
	else {
		process.Init(
			objectName,
			LegacyType::WithTexture,
			texturePath
		);

		model = Plane::MakeTesselatedTextured(1, 1);
		process.ProcessLegacyType(gfx, model, 3u);
	}

	AddBind(process.GetPipelineState());
	AddBind(process.GetRootSignature());
	AddBind(process.GetTopology());
	AddBind(process.GetVertexBuffer());
	AddBind(process.GetIndexBuffer());

	if (texturePath == "") {
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
	}
	else
		AddBind(process.GetTextures());

	AddBind(
		std::make_unique<ConstantBufferMat>(
			0u, 16u, std::bind(&Transform::GetTransformWithProjectionCM, &m_transform)
			)
	);

	// Slot 1 is reserved for Light Buffer

	AddBind(
		std::make_unique<ConstantBufferCBVDynamic<DirectX::XMMATRIX>>(
			3u, std::bind(&Transform::GetTransformCM, &m_transform)
			)
	);

	m_transform = DirectX::XMMatrixTranslation(1.0f, 1.0f, 1.0f);
}

void Quad::SetTransform(const DirectX::XMMATRIX& transform) noexcept {
	m_transform = transform;
}
