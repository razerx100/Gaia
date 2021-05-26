#include <SkinnedBox.hpp>
#include <Cube.hpp>
#include <Texture.hpp>
#include <Surface.hpp>
#include <BindAll.hpp>

SkinnedBox::SkinnedBox(Graphics& gfx,
		std::mt19937& rng,
		std::uniform_real_distribution<float>& adist,
		std::uniform_real_distribution<float>& ddist,
		std::uniform_real_distribution<float>& odist,
		std::uniform_real_distribution<float>& rdist)
	:
	r(rdist(rng)),
	roll(0.0f),
	pitch(0.0f),
	yaw(0.0f),
	theta(adist(rng)),
	phi(adist(rng)),
	chi(adist(rng)),
	droll(ddist(rng)),
	dpitch(ddist(rng)),
	dyaw(ddist(rng)),
	dtheta(odist(rng)),
	dphi(odist(rng)),
	dchi(odist(rng)) {

	if (!IsDataInitialized()) {

		std::vector<D3D12_INPUT_ELEMENT_DESC> inputDescs = {
			{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"TexCoord", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		};

		PSODesc pso = PSODesc();

		pso.SetInputLayout(std::move(inputDescs));

		std::unique_ptr<RootSignature> rootSig = std::make_unique<RootSignature>(
			gfx, s_ShaderPath + L"RS_VS_CBuff_Tex2D.cso"
			);

		pso.SetRootSignature(rootSig.get());

		pso.SetPixelShader(s_ShaderPath + L"PSTexture2D.cso");

		pso.SetVertexShader(s_ShaderPath + L"VSTexture2D.cso");

		std::unique_ptr<Topology> topo = std::make_unique<Topology>(
			D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
			D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST
			);

		pso.SetTopology(topo.get());

		AddStaticBind(std::make_unique<PipelineState>(gfx, pso));

		AddStaticBind(std::move(rootSig));

		AddStaticBind(std::move(topo));

		IndexedTriangleList model = Cube::MakeSkinned();

		std::vector<DirectX::XMFLOAT2> uvCoord = {
			{2.0f / 3.0f, 0.0f},
			{1.0f / 3.0f, 0.0f},
			{2.0f / 3.0f, 1.0f / 4.0f},
			{1.0f / 3.0f, 1.0f / 4.0f},
			{2.0f / 3.0f, 3.0f / 4.0f},
			{1.0f / 3.0f, 3.0f / 4.0f},
			{2.0f / 3.0f, 2.0f / 4.0f},
			{1.0f / 3.0f, 2.0f / 4.0f},
			{2.0f / 3.0f, 1.0f},
			{1.0f / 3.0f, 1.0f},
			{1.0f, 1.0f / 4.0f},
			{1.0f, 2.0f / 4.0f},
			{0.0f, 1.0f / 4.0f},
			{0.0f, 2.0f / 4.0f}
		};

		AddStaticBind(std::make_unique<Texture>(gfx, Surface::FromFile("Image\\cube.png")));

		AddStaticBind(std::make_unique<VertexBuffer>(
			std::move(model.m_Vertices), std::move(uvCoord)));

		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(std::move(model.m_Indices)));
	}

	AddBind(std::make_unique<ConstantBufferMat>(
		0u, 16u, std::bind(&Transform::GetTransformWithProjectionCM, &m_Transform))
	);
}

void SkinnedBox::Update(float deltaTime) noexcept {

	roll += droll * deltaTime;
	pitch += dpitch * deltaTime;
	yaw += dyaw * deltaTime;
	theta += dtheta * deltaTime;
	phi += dphi * deltaTime;
	chi += dchi * deltaTime;

	m_Transform =
		DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
		DirectX::XMMatrixTranslation(r, 0.0f, 0.0f) *
		DirectX::XMMatrixRotationRollPitchYaw(theta, phi, chi);
}
