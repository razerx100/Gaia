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

	if (texturePath != "") {
		process.Init(
			objectName,
			LegacyType::WithTexture,
			texturePath
		);

		model = Plane::MakeTesselatedTextured(1, 1);
		process.ProcessLegacyType(gfx, model, 2u);
	}
	else {
		process.Init(
			objectName,
			LegacyType::SolidColor
		);

		model = Plane::MakeTesselated(1, 1);
		process.ProcessLegacyType(gfx, model);
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
			float specularIntensity;
			float specularPower;
		};

		ConstantBufferColor materialColor = {
			{1.0f, 1.0f, 1.0f, 1.0f},
			0.6f,
			90.0f
		};

		AddBind(BindProcessor::GetOrAddGenericCBuffer
			<ConstantBufferCBVStatic<ConstantBufferColor>>(
				"CBVStaticWhiteWithSpec",
				2u, &materialColor
				)
		);

	}
	else {
		AddBind(process.GetTextures());

		struct ConstantBufferSpecular {
			float specularIntensity;
			float specularPower;
		};

		ConstantBufferSpecular specularDetails = {
			0.1f,
			90.0f
		};

		AddBind(BindProcessor::GetOrAddGenericCBuffer
			<ConstantBufferCBVStatic<ConstantBufferSpecular>>(
				"CBVStaticSpecularDetails",
				3u, &specularDetails
				)
		);
	}

	// Slot 1 is reserved for Light Buffer

	AddBind(std::make_unique<ConstantBufferCBVDynamic<TransformMatrices>>(
		0u, std::bind(&Transform::GetTransforms, &m_transform)
		)
	);

	m_transform = DirectX::XMMatrixTranslation(1.0f, 1.0f, 1.0f);
}

void Quad::SetTransform(const DirectX::XMMATRIX& transform) noexcept {
	m_transform = transform;
}
