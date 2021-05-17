#include <SolidSphere.hpp>
#include <Graphics.hpp>
#include <Sphere.hpp>
#include <BindAll.hpp>

SolidSphere::SolidSphere(Graphics& gfx, float radius) {
	if (!IsDataInitialized()) {
		std::vector<D3D12_INPUT_ELEMENT_DESC> inputDescs = {
			{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		};

		InputLayout inputLayout = InputLayout(std::move(inputDescs));

		std::unique_ptr<RootSignature> rootSig = std::make_unique<RootSignature>(
			gfx, s_ShaderPath + L"RSSolidColor.cso"
			);

		Shader pixel = Shader(s_ShaderPath + L"PSSolidColor.cso");

		Shader vertex = Shader(s_ShaderPath + L"VSSolidColor.cso");

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
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		psoDesc.SampleDesc.Count = 1;

		AddStaticBind(std::make_unique<PipelineState>(gfx, &psoDesc));

		AddStaticBind(std::move(rootSig));

		AddStaticBind(std::move(topo));

		IndexedTriangleList model = Sphere::Make();

		model.Transform(DirectX::XMMatrixScaling(radius, radius, radius));

		AddStaticBind(std::make_unique<VertexBuffer>(std::move(model.m_Vertices)));

		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(std::move(model.m_Indices)));

		struct ConstantBufferColor {
			struct {
				float red;
				float green;
				float blue;
				float alpha;
			}face_color;
		};

		ConstantBufferColor FaceColor = {
			{1.0f, 1.0f, 1.0f, 1.0f}
		};

		std::uint8_t* cpuPtr = nullptr;

		AddStaticBind(std::make_unique<PixelConstantBuffer<ConstantBufferColor>>(
			1u, static_cast<std::uint32_t>(sizeof(FaceColor)), &cpuPtr
			));

		memcpy(cpuPtr, &FaceColor, sizeof(FaceColor));
	}

	AddBind(std::make_unique<VertexConstantBuffer>(
		0u, 16u, std::bind(&SolidSphere::GetTransformationMatrix, this)
		));

	m_Transform = DirectX::XMMatrixTranslation(1.0f, 1.0f, 1.0f);
}

void SolidSphere::Update(float deltatime) noexcept {}

void SolidSphere::SetPosition(DirectX::XMFLOAT3 position) noexcept {
	m_Transform = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
}
