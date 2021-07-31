#include <Quad.hpp>
#include <Graphics.hpp>
#include <BindableProcessor.hpp>
#include <Utilities.hpp>
#include <BindAll.hpp>
#include <Plane.hpp>

Quad::Quad(
	Graphics& gfx,
	const std::string& objectName,
	const std::string& texturePath,
	const std::string& normalMapPath
)
	: Drawable(objectName) {

	BindProcessor process;
	IndexedTriangleList model;

	if (texturePath != "") {
		if (normalMapPath != "") {
			process.Init(
				objectName,
				LegacyType::WithTexture,
				texturePath,
				normalMapPath
			);
		}
		else {
			process.Init(
				objectName,
				LegacyType::WithTexture,
				texturePath
			);
		}

		model = Plane::MakeTesselatedTextured(1, 1);
		process.ProcessLegacyType(gfx, model, 3u);
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

	std::uint8_t transformRootIndex;

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
			0.8f,
			35.0f
		};

		AddBind(BindProcessor::GetOrAddGenericCBuffer
			<ConstantBufferCBVStatic<ConstantBufferColor>>(
				"CBVStaticWhiteWithSpec",
				2u, &materialColor
				)
		);

		transformRootIndex = 3u;
	}
	else {
		AddBind(process.GetTextures());

		struct ConstantBufferSpecular {
			float specularIntensity;
			float specularPower;
		};

		ConstantBufferSpecular specularDetails = {
			0.1f,
			20.0f
		};

		AddBind(BindProcessor::GetOrAddGenericCBuffer
			<ConstantBufferCBVStatic<ConstantBufferSpecular>>(
				"CBVStaticSpecularDetails",
				4u, &specularDetails
				)
		);

		transformRootIndex = 2u;
	}

	AddBind(
		std::make_unique<ConstantBufferMat>(
			0u, 16u, std::bind(&Transform::GetTransformWithProjectionCM, &m_transform)
			)
	);

	// Slot 1 is reserved for Light Buffer

	AddBind(
		std::make_unique<ConstantBufferCBVDynamic<DirectX::XMMATRIX>>(
			transformRootIndex, std::bind(&Transform::GetTransformCM, &m_transform)
			)
	);

	m_transform = DirectX::XMMatrixTranslation(1.0f, 1.0f, 1.0f);
}

void Quad::SetTransform(const DirectX::XMMATRIX& transform) noexcept {
	m_transform = transform;
}
