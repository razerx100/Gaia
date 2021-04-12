#include <Pyramid.hpp>
#include <Cone.hpp>
#include <BindableAll.hpp>

Pyramid::Pyramid(Graphics& gfx,
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

		IndexedTriangleList model = Cone::MakeTesselated(4u);

		std::vector<DirectX::XMFLOAT4> vertexColors(model.m_Vertices.size());

		std::uniform_real_distribution<float> cdist{-0.2, 1.0f};

		for (DirectX::XMFLOAT4& vColor : vertexColors)
			vColor = {cdist(rng), cdist(rng), cdist(rng), 1.0f};

		VertexConstantBuffer::SetBuffer(gfx);

		AddStaticBind(std::make_unique<VertexBuffer>(
			gfx, std::move(model.m_Vertices), std::move(vertexColors)
			));

		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, model.m_Indices));

		AddStaticBind(std::make_unique<PixelShader>(gfx, s_ShaderPath + L"PColorFromVS.cso"));

		std::unique_ptr<VertexShader> vShader = std::make_unique<VertexShader>(gfx, s_ShaderPath + L"VWithColor.cso");

		const std::vector<D3D11_INPUT_ELEMENT_DESC> inputDescs = {
			{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"Color", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12u, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

		AddStaticBind(std::make_unique<VertexLayout>(gfx, inputDescs, vShader->GetByteCode()));

		AddStaticBind(std::move(vShader));

		AddStaticBind(std::make_unique<Topology>(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}

	AddBind(std::make_unique<VertexConstantBuffer>(*this));
}

void Pyramid::Update(float deltaTime) noexcept {

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
