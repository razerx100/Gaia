#include <Box.hpp>
#include <Cube.hpp>
#include <BindAll.hpp>

Box::Box(Graphics& gfx,
		std::mt19937& rng,
		std::uniform_real_distribution<float>& adist,
		std::uniform_real_distribution<float>& ddist,
		std::uniform_real_distribution<float>& odist,
		std::uniform_real_distribution<float>& rdist,
		std::uniform_real_distribution<float>& bdist)
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
		};

		InputLayout inputLayout = InputLayout(std::move(inputDescs));

		std::unique_ptr<RootSignature> rootSig = std::make_unique<RootSignature>(
			gfx, s_ShaderPath + L"RS_VS_PS_CBuff.cso"
			);

		Shader pixel = Shader(s_ShaderPath + L"PSFaceColor.cso");

		Shader vertex = Shader(s_ShaderPath + L"VSFaceColor.cso");

		std::unique_ptr<Topology> topo = std::make_unique<Topology>(
			D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
			D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST
			);

		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
		psoDesc.InputLayout = inputLayout.GetInputLayout();
		psoDesc.pRootSignature = rootSig->GetSignature();
		psoDesc.VS = vertex.GetShaderByteCode();
		psoDesc.PS = pixel.GetShaderByteCode();
		psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		psoDesc.DepthStencilState.DepthEnable = TRUE;
		psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		psoDesc.DepthStencilState.StencilEnable = FALSE;
		psoDesc.SampleMask = UINT_MAX;
		psoDesc.PrimitiveTopologyType = topo->GetTopology();
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_B8G8R8A8_UNORM;
		psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		psoDesc.SampleDesc.Count = 1;

		AddStaticBind(std::make_unique<PipelineState>(gfx, &psoDesc));

		AddStaticBind(std::move(rootSig));

		AddStaticBind(std::move(topo));

		IndexedTriangleList model = Cube::Make();

		AddStaticBind(std::make_unique<VertexBuffer>(gfx, std::move(model.m_Vertices)));

		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, std::move(model.m_Indices)));

		struct ConstantBufferColor {
			struct {
				float red;
				float green;
				float blue;
				float alpha;
			}face_color[6];
		};

		ConstantBufferColor constBufferC = {
			{
			{1.0f, 0.0f, 0.0f, 1.0f},
			{0.0f, 1.0f, 1.0f, 1.0f},
			{0.0f, 1.0f, 0.0f, 1.0f},
			{0.0f, 0.0f, 1.0f, 1.0f},
			{1.0f, 0.0f, 1.0f, 1.0f},
			{0.0f, 0.75f, 0.5f, 1.0f}
			}
		};

		ConstantBuffer<ConstantBufferColor>::SetData(std::move(constBufferC));

		AddStaticBind(std::make_unique<ConstantBuffer<ConstantBufferColor>>(
			1u, 24u, *this
			));
	}

	AddBind(std::make_unique<VertexConstantBuffer>(0u, 16u, *this));

	DirectX::XMStoreFloat3x3(
		&mt,
		DirectX::XMMatrixScaling(1.0f, 1.0f, bdist(rng))
	);
}

void Box::Update(float deltaTime) noexcept {

	roll += droll * deltaTime;
	pitch += dpitch * deltaTime;
	yaw += dyaw * deltaTime;
	theta += dtheta * deltaTime;
	phi += dphi * deltaTime;
	chi += dchi * deltaTime;

	m_Transform =
		DirectX::XMLoadFloat3x3(&mt) *
		DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
		DirectX::XMMatrixTranslation(r, 0.0f, 0.0f) *
		DirectX::XMMatrixRotationRollPitchYaw(theta, phi, chi);
}
