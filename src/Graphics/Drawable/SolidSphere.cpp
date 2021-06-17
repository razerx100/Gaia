#include <SolidSphere.hpp>
#include <Graphics.hpp>
#include <Sphere.hpp>
#include <BindAll.hpp>
#include <App.hpp>

SolidSphere::SolidSphere(Graphics& gfx, float radius) {
	if (!IsDataInitialized()) {
		PSODesc pso = PSODesc();

		VertexLayout vertexLayout = {
			{"Position", 12u}
		};

		pso.SetInputLayout(vertexLayout);

		std::unique_ptr<RootSignature> rootSig = std::make_unique<RootSignature>(
			gfx, App::GetShaderPath() + L"RSSolidColor.cso"
			);

		pso.SetRootSignature(rootSig.get());

		pso.SetPixelShader(App::GetShaderPath() + L"PSSolidColor.cso");

		pso.SetVertexShader(App::GetShaderPath() + L"VSSolidColor.cso");

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

		AddStaticBind(std::make_unique<VertexBuffer>(
			model.GetVerticesObject(vertexLayout))
		);

		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(model.GetIndices()));

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
			)
		);
	}

	AddBind(std::make_unique<ConstantBufferMat>(
		0u, 16u, std::bind(&Transform::GetTransformWithProjectionCM, &m_Transform)
		)
	);

	m_Transform = DirectX::XMMatrixTranslation(1.0f, 1.0f, 1.0f);
}

void SolidSphere::SetPosition(DirectX::XMFLOAT3 position) noexcept {
	m_Transform = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
}
