#include <Test/Box.hpp>
#include <Cube.hpp>
#include <BindAll.hpp>
#include <PSODesc.hpp>
#include <Utilities.hpp>

Box::Box(Graphics& gfx,
	const std::string& name,
	const std::string& texPath
) : Drawable(name) {

	PSODesc pso = PSODesc();

	VertexLayout vertexLayout = {
		{"Position", 12u},
		{"Normal", 12u}
	};

	pso.SetInputLayout(vertexLayout);

	std::unique_ptr<RootSignature> rootSig =
		std::make_unique<RootSignaturePreCompiled>(
			gfx, GUtil::ShaderPath::Get() + "RSTest.cso"
			);

	pso.SetRootSignature(rootSig.get());

	pso.SetPixelShader(GUtil::ShaderPath::Get() + "PSTest.cso");

	pso.SetVertexShader(GUtil::ShaderPath::Get() + "VSTest.cso");

	std::unique_ptr<Topology> topo = std::make_unique<Topology>("TriangleList");

	pso.SetTopology(topo.get());

	AddBind(std::make_unique<PipelineState>(gfx, pso));

	AddBind(std::move(rootSig));

	AddBind(std::move(topo));

	IndexedTriangleList model = Cube::MakeIndependent();
	model.SetNormalsIndependentFlat();

	AddBind(std::make_unique<VertexBuffer>(
		model.GetVerticesObject(vertexLayout, LegacyType::SolidColor))
	);

	AddBind(std::make_unique<IndexBuffer>(model.GetIndices()));

	AddBind(std::make_unique<ConstantBufferCBVDynamic<TransformMatrices>>(
		0u, std::bind(&Transform::GetTransforms, &m_transform)
		)
	);

	struct ConstantBufferColor {
		struct {
			float red;
			float green;
			float blue;
			float alpha;
		}fill_color;
		float specularIntensity;
		float specularPower;
	};

	ConstantBufferColor materialColor = {
		{0.50001f, 0.50001f, 0.00001f, 1.0f},
		0.6f,
		90.0f
	};

	AddBind(
		std::make_unique<ConstantBufferCBVStatic<ConstantBufferColor>>(
			2u, &materialColor
			)
	);
}

void Box::SetTransform(
	const DirectX::XMMATRIX& accumulatedTransform
) noexcept {
	m_transform = accumulatedTransform;
}
