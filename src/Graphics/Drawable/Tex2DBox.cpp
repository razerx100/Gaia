#include <Tex2DBox.hpp>
#include <Cube.hpp>
#include <App.hpp>
#include <Texture.hpp>
#include <Surface.hpp>
#include <BindAll.hpp>

Tex2DBox::Tex2DBox(Graphics& gfx,
	std::mt19937& rng,
	std::uniform_real_distribution<float>& adist,
	std::uniform_real_distribution<float>& ddist,
	std::uniform_real_distribution<float>& odist,
	std::uniform_real_distribution<float>& rdist)
	:
	m_tobj(rng, adist, ddist, odist, rdist) {

	if (!IsDataInitialized()) {
		PSODesc pso = PSODesc();

		VertexLayout vertexLayout = {
			{"Position", 12u},
			{"Normal", 12u},
			{"TexCoord", 8u}
		};

		pso.SetInputLayout(vertexLayout);

		std::unique_ptr<RootSignature> rootSig = std::make_unique<RootSignature>(
			gfx, s_ShaderPath + L"RSPixelLightTex2D.cso"
			);

		pso.SetRootSignature(rootSig.get());

		pso.SetPixelShader(s_ShaderPath + L"PSPixelLightTex2D.cso");

		pso.SetVertexShader(s_ShaderPath + L"VSPixelLightTex2D.cso");

		std::unique_ptr<Topology> topo = std::make_unique<Topology>(
			D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
			D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST
			);

		pso.SetTopology(topo.get());

		AddStaticBind(std::make_unique<PipelineState>(gfx, pso));

		AddStaticBind(std::move(rootSig));

		AddStaticBind(std::move(topo));

		IndexedTriangleList model = Cube::MakeIndependent();

		std::vector<DirectX::XMFLOAT2> uvCoord = {
			{0.0f, 0.0f},
			{1.0f, 0.0f},
			{0.0f, 1.0f},
			{1.0f, 1.0f},
			{0.0f, 0.0f},
			{1.0f, 0.0f},
			{0.0f, 1.0f},
			{1.0f, 1.0f},
			{0.0f, 0.0f},
			{1.0f, 0.0f},
			{0.0f, 1.0f},
			{1.0f, 1.0f},
			{0.0f, 0.0f},
			{1.0f, 0.0f},
			{0.0f, 1.0f},
			{1.0f, 1.0f},
			{0.0f, 0.0f},
			{1.0f, 0.0f},
			{0.0f, 1.0f},
			{1.0f, 1.0f},
			{0.0f, 0.0f},
			{1.0f, 0.0f},
			{0.0f, 1.0f},
			{1.0f, 1.0f}
		};

		AddStaticBind(std::make_unique<Texture>(gfx, Surface::FromFile("Image\\pupa_gun.png"), 4u));

		model.SetNormalsIndependentFlat();

		AddStaticBind(std::make_unique<VertexBuffer>(
			std::move(model.m_Vertices), std::move(model.m_Normals), std::move(uvCoord)));

		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(std::move(model.m_Indices)));

		AddStaticBind(std::make_unique<ConstantBuffer<LightData>>(
			3u, static_cast<std::uint32_t>(sizeof(LightData) / 4u),
			std::bind(&Light::GetLightData, App::GetLight()))
		);
	}

	struct PSMaterial {
		float specularIntensity;
		float specularPower;
	};

	PSMaterial matData = {
		0.6f,
		30.0f
	};

	AddBind(std::make_unique<ConstantBufferCBVStatic<PSMaterial>>(
		2u, &matData)
	);

	AddBind(std::make_unique<ConstantBufferMat>(
		0u, 16u, std::bind(&Transform::GetTransformWithProjectionCM, &m_Transform))
	);

	AddBind(std::make_unique<ConstantBufferCBVDynamic<DirectX::XMMATRIX>>(
		1u, std::bind(&Transform::GetTransformCM, &m_Transform))
	);
}

void Tex2DBox::Update(float deltaTime) noexcept {
	m_Transform = m_tobj.GetMomentum(deltaTime);
}
