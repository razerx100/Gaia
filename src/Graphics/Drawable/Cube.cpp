#include <Cube.hpp>

Cube::Cube(Graphics& gfx, float posX, float posY)
	: m_PosX(posX), m_PosY(posY) {

	struct Vertex {
		float x;
		float y;
		float z;
	};

	const std::vector<Vertex> vertices = {
		{-1.0f, -1.0f, -1.0f},
		{1.0f, -1.0f, -1.0f},
		{-1.0f, 1.0f, -1.0f},
		{1.0f, 1.0f, -1.0f},
		{-1.0f, -1.0f, 1.0f},
		{1.0f, -1.0f, 1.0f},
		{-1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},
	};

	AddBind(std::make_unique<VertexBuffer>(gfx, vertices));

	const std::vector<unsigned short> indices = {
		0u, 2u, 1u,		2u, 3u, 1u,
		1u, 3u, 5u,		3u, 7u, 5u,
		2u, 6u, 3u,		6u, 7u, 3u,
		4u, 5u, 7u,		4u, 7u, 6u,
		0u, 4u, 2u,		2u, 4u, 6u,
		0u, 1u, 4u,		1u, 5u, 4u
	};

	AddIndexBuffer(std::make_unique<IndexBuffer>(gfx, indices));

	AddCVertexBuffer(std::make_unique<VertexConstantBuffer<DirectX::XMMATRIX>>(gfx));

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

	AddBind(std::make_unique<PixelConstantBuffer<ConstantBufferColor>>(gfx, constBufferC));

	AddBind(std::make_unique<PixelShader>(gfx, m_ShaderPath + L"TPixelShader.cso"));

	std::unique_ptr<VertexShader> vShader = std::make_unique<VertexShader>(gfx, m_ShaderPath + L"TVertexShader.cso");

	const std::vector<D3D11_INPUT_ELEMENT_DESC> inputDescs = {
		{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	AddBind(std::make_unique<VertexLayout>(gfx, inputDescs, vShader->GetByteCode()));

	AddBind(std::move(vShader));

	AddBind(std::make_unique<Topology>(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
}

void Cube::Update(Graphics& gfx, float deltaTime) noexcept {

	DirectX::XMMATRIX constBufferT =
			DirectX::XMMatrixRotationX(deltaTime) *
			DirectX::XMMatrixRotationY(deltaTime) *
			DirectX::XMMatrixRotationZ(deltaTime) *
			DirectX::XMMatrixTranslation(m_PosX, m_PosY, 8.0f) *
			DirectX::XMMatrixPerspectiveLH(1.0f, 9.0f / 16.0f, 0.5f, 10.0f);

	m_VertexCBuffer->Update(
		gfx, &constBufferT
	);
}
