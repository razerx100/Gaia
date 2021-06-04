#include <SolidSphere.hpp>
#include <Graphics.hpp>
#include <Sphere.hpp>
#include <BindAll.hpp>

SolidSphere::SolidSphere(Graphics& gfx, float radius) {
	if (!IsDataInitialized()) {
		PSODesc pso = PSODesc();

		VertexLayout vertexLayout = {
			{"Position", 12u}
		};

		pso.SetInputLayout(vertexLayout);

		std::unique_ptr<RootSignature> rootSig = std::make_unique<RootSignature>(
			gfx, s_ShaderPath + L"RSSolidColor.cso"
			);

		pso.SetRootSignature(rootSig.get());

		pso.SetPixelShader(s_ShaderPath + L"PSSolidColor.cso");

		pso.SetVertexShader(s_ShaderPath + L"VSSolidColor.cso");

		std::unique_ptr<Topology> topo = std::make_unique<Topology>(
			D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
			D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST
			);

		pso.SetTopology(topo.get());

		AddStaticBind(std::make_unique<PipelineState>(gfx, pso));

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

		ConstantBufferColor materialColor = {
			{1.0f, 1.0f, 1.0f, 1.0f}
		};

		AddStaticBind(std::make_unique<ConstantBufferCBVStatic<ConstantBufferColor>>(
			1u, &materialColor
			));
	}

	AddBind(std::make_unique<ConstantBufferMat>(
		0u, 16u, std::bind(&Transform::GetTransformWithProjectionCM, &m_Transform)
		));

	m_Transform = DirectX::XMMatrixTranslation(1.0f, 1.0f, 1.0f);
}

void SolidSphere::Update(float deltatime) noexcept {}

void SolidSphere::SetPosition(DirectX::XMFLOAT3 position) noexcept {
	m_Transform = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
}
