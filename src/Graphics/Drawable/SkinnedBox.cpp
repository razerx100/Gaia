#include <SkinnedBox.hpp>
#include <Cube.hpp>
#include <GraphicsThrowMacros.hpp>
#include <Texture.hpp>
#include <Surface.hpp>
#include <Sampler.hpp>
#include <BindableAll.hpp>

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

		AddStaticBind(std::make_unique<Sampler>(gfx));

		VertexConstantBuffer::SetBuffer(gfx);

		AddStaticBind(std::make_unique<VertexBuffer>(
			gfx, std::move(model.m_Vertices), std::move(uvCoord)));

		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, model.m_Indices));

		std::vector<D3D11_INPUT_ELEMENT_DESC> inputDescs = {
			{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TexCoord", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		};

		AddStaticBind(std::make_unique<PixelShader>(gfx, s_ShaderPath + L"PTexture.cso"));

		std::unique_ptr<VertexShader> vShader = std::make_unique<VertexShader>(gfx, s_ShaderPath + L"VTexture.cso");

		AddStaticBind(std::make_unique<VertexLayout>(gfx, inputDescs, vShader->GetByteCode()));

		AddStaticBind(std::move(vShader));

		AddStaticBind(std::make_unique<Topology>(
				D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST
			));
	}

	AddBind(std::make_unique<VertexConstantBuffer>(*this));
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
		DirectX::XMMatrixRotationRollPitchYaw(theta, phi, chi) *
		DirectX::XMMatrixTranslation(0.0f, 0.0f, 20.0f) *
		DirectX::XMMatrixPerspectiveLH(1.0f, 9.0f / 16.0f, 0.5f, 40.0f);
}
