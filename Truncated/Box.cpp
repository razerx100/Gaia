#include <Box.hpp>
#include <Cube.hpp>
#include <BindAll.hpp>
#include <App.hpp>

Box::Box(Graphics& gfx,
	DirectX::XMFLOAT4 material) {

	if (!IsDataInitialized()) {
		PSODesc pso = PSODesc();

		VertexLayout vertexLayout = {
			{"Position", 12u},
			{"Normal", 12u}
		};

		pso.SetInputLayout(vertexLayout);

		std::unique_ptr<RootSignature> rootSig = std::make_unique<RootSignature>(
			gfx, App::GetShaderPath() + L"RSPixelLight.cso"
			);

		pso.SetRootSignature(rootSig.get());

		pso.SetPixelShader(App::GetShaderPath() + L"PSPixelLight.cso");

		pso.SetVertexShader(App::GetShaderPath() + L"VSPixelLight.cso");

		std::unique_ptr<Topology> topo = std::make_unique<Topology>(
			D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
			D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST
			);

		pso.SetTopology(topo.get());

		AddStaticBind(std::make_unique<PipelineState>(gfx, pso));

		AddStaticBind(std::move(rootSig));

		AddStaticBind(std::move(topo));

		IndexedTriangleList model = Cube::MakeIndependent();

		model.SetNormalsIndependentFlat();

		AddStaticBind(std::make_unique<VertexBuffer>(
			model.GetVerticesObject(vertexLayout, true))
		);

		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(model.GetIndices()));

		AddStaticBind(std::make_unique<ConstantBuffer<LightData>>(
			3u, static_cast<std::uint32_t>(sizeof(LightData) / 4u),
			std::bind(&Light::GetLightData, App::GetLight())
			)
		);
	}

	struct PSMaterial {
		DirectX::XMFLOAT4 material;
		float specularIntensity;
		float specularPower;
	};

	PSMaterial matData = {
		material,
		0.6f,
		30.0f
	};

	AddBind(std::make_unique<ConstantBufferCBVStatic<PSMaterial>>(
		2u, &matData
		)
	);

	AddBind(std::make_unique<ConstantBufferMat>(
		0u, 16u, std::bind(&Transform::GetTransformWithProjectionCM, &m_Transform)
		)
	);

	AddBind(std::make_unique<ConstantBufferCBVDynamic<DirectX::XMMATRIX>>(
		1u, std::bind(&Transform::GetTransformCM, &m_Transform)
		)
	);
}

void Box::Draw(
	Graphics& gfx,
	const DirectX::XMMATRIX& accumulatedTransform
) noexcept {
	m_Transform = accumulatedTransform;

	DrawableBase::Draw(gfx);
}
