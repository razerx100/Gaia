#include <Pyramid.hpp>
#include <Cone.hpp>
#include <BindAll.hpp>

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

		std::vector<D3D12_INPUT_ELEMENT_DESC> inputDescs = {
			{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"Color", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12u, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		};

		InputLayout inputLayout = InputLayout(std::move(inputDescs));

		std::unique_ptr<RootSignature> rootSig = std::make_unique<RootSignature>(
			gfx, s_ShaderPath + L"RS_VS_CBuff.cso"
			);

		Shader pixel = Shader(s_ShaderPath + L"PSVertexColor.cso");

		Shader vertex = Shader(s_ShaderPath + L"VSVertexColor.cso");

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

		IndexedTriangleList model = Cone::MakeTesselated(4u);

		std::vector<DirectX::XMFLOAT4> vertexColors(model.m_Vertices.size());

		std::uniform_real_distribution<float> cdist{ -0.2, 1.0f };

		for (DirectX::XMFLOAT4& vColor : vertexColors)
			vColor = { cdist(rng), cdist(rng), cdist(rng), 1.0f };

		AddStaticBind(std::make_unique<VertexBuffer>(
			gfx, std::move(model.m_Vertices), std::move(vertexColors)
			));

		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, std::move(model.m_Indices)));
	}

	AddBind(std::make_unique<VertexConstantBuffer>(0u, 16u, std::bind(&Pyramid::GetTransformationMatrix, this)));
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
