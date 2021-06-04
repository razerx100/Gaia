#include <Cylinder.hpp>
#include <Prism.hpp>
#include <BindAll.hpp>
#include <App.hpp>

Cylinder::Cylinder(
	Graphics& gfx,
	std::mt19937& rng,
	std::uniform_real_distribution<float>& adist,
	std::uniform_real_distribution<float>& ddist,
	std::uniform_real_distribution<float>& odist,
	std::uniform_real_distribution<float>& rdist,
	std::uniform_int_distribution<int>& tdist,
	DirectX::XMFLOAT4 material)
	: m_tobj(rng, adist, ddist, odist, rdist) {
	if (!IsDataInitialized()) {
		PSODesc pso = PSODesc();

		VertexLayout vertexLayout = {
			{"Position", 12u},
			{"Normal", 12u}
		};

		pso.SetInputLayout(vertexLayout);

		std::unique_ptr<RootSignature> rootSig = std::make_unique<RootSignature>(
			gfx, s_ShaderPath + L"RSPixelLight.cso"
			);

		pso.SetRootSignature(rootSig.get());

		pso.SetPixelShader(s_ShaderPath + L"PSPixelLight.cso");

		pso.SetVertexShader(s_ShaderPath + L"VSPixelLight.cso");

		std::unique_ptr<Topology> topo = std::make_unique<Topology>(
			D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
			D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST
			);

		pso.SetTopology(topo.get());

		AddStaticBind(std::make_unique<PipelineState>(gfx, pso));

		AddStaticBind(std::move(rootSig));

		AddStaticBind(std::move(topo));

		AddStaticBind(std::make_unique<ConstantBuffer<LightData>>(
			3u, static_cast<std::uint32_t>(sizeof(LightData) / 4u),
			std::bind(&Light::GetLightData, App::GetLight())
			));
	}

	IndexedTriangleList model = Prism::MakeTesselatedIndependentCapNormals(tdist(rng));

	AddBind(std::make_unique<VertexBuffer>(
		std::move(model.m_Vertices), std::move(model.m_Normals)
		));

	AddIndexBuffer(std::make_unique<IndexBuffer>(std::move(model.m_Indices)));

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
		));

	AddBind(std::make_unique<ConstantBufferMat>(
		0u, 16u, std::bind(&Transform::GetTransformWithProjectionCM, &m_Transform)
		));

	AddBind(std::make_unique<ConstantBufferCBVDynamic<DirectX::XMMATRIX>>(
		1u, std::bind(&Transform::GetTransformCM, &m_Transform)
		));
}

void Cylinder::Update(float deltaTime) noexcept {
	m_Transform = m_tobj.GetMomentum(deltaTime);
}

std::uint32_t Cylinder::GetIndexCount() const noexcept {
	return m_IndexCount;
}
