#include <Melon.hpp>
#include <Sphere.hpp>
#include <BindableAll.hpp>

Melon::Melon(Graphics& gfx,
		std::mt19937& rng,
		std::uniform_real_distribution<float>& adist,
		std::uniform_real_distribution<float>& ddist,
		std::uniform_real_distribution<float>& odist,
		std::uniform_real_distribution<float>& rdist,
		std::uniform_int_distribution<int>& longdist,
		std::uniform_int_distribution<int>& latdist)
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
		struct ConstantBufferColor {
			struct {
				float red;
				float green;
				float blue;
				float alpha;
			}face_color[6];
		};

		const ConstantBufferColor constBufferC = {
			{
			{1.0f, 0.0f, 0.0f, 1.0f},
			{0.0f, 1.0f, 1.0f, 1.0f},
			{0.0f, 1.0f, 0.0f, 1.0f},
			{0.0f, 0.0f, 1.0f, 1.0f},
			{1.0f, 0.0f, 1.0f, 1.0f},
			{0.0f, 0.75f, 0.5f, 1.0f}
			}
		};

		PixelConstantBuffer<ConstantBufferColor>::SetBuffer(gfx, constBufferC);

		VertexConstantBuffer::SetBuffer(gfx);

		AddStaticBind(std::make_unique<PixelConstantBuffer<ConstantBufferColor>>(*this));

		AddStaticBind(std::make_unique<PixelShader>(gfx, s_ShaderPath + L"PConstantFaceColors.cso"));

		std::unique_ptr<VertexShader> vShader = std::make_unique<VertexShader>(gfx, s_ShaderPath + L"VNothingExtra.cso");

		const std::vector<D3D11_INPUT_ELEMENT_DESC> inputDescs = {
			{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		};

		AddStaticBind(std::make_unique<VertexLayout>(gfx, inputDescs, vShader->GetByteCode()));

		AddStaticBind(std::move(vShader));

		AddStaticBind(std::make_unique<Topology>(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}

	IndexedTriangleList model = Sphere::MakeTesselated(
		static_cast<std::uint16_t>(latdist(rng)), static_cast<std::uint16_t>(longdist(rng))
	);

	AddBind(std::make_unique<VertexConstantBuffer>(*this));

	AddBind(std::make_unique<VertexBuffer>(gfx, std::move(model.m_Vertices)));

	AddIndexBuffer(std::make_unique<IndexBuffer>(gfx, model.m_Indices));
}

void Melon::Update(float deltaTime) noexcept {

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

std::uint32_t Melon::GetIndexCount() const noexcept {
	return m_IndexCount;
}
