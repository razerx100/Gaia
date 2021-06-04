#include <Model.hpp>
#include <Cube.hpp>
#include <BindAll.hpp>
#include <App.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

Model::Model(Graphics& gfx,
	std::mt19937& rng,
	std::uniform_real_distribution<float>& adist,
	std::uniform_real_distribution<float>& ddist,
	std::uniform_real_distribution<float>& odist,
	std::uniform_real_distribution<float>& rdist,
	DirectX::XMFLOAT4 material, float scale)
	: m_tobj(rng, adist, ddist, odist, rdist) {

	if (!IsDataInitialized()) {
		PSODesc pso = PSODesc();

		VertexLayout vertexLayout = {
			{"Position", 12u},
			{"Normal", 12u}
		};

		pso.SetInputLayout(vertexLayout);

		std::unique_ptr<RootSignature> rootSig = std::make_unique<RootSignature>(
			gfx, s_ShaderPath + L"RSPixelLight.cso"
			);

		pso.SetRootSignature(rootSig.get());

		pso.SetPixelShader(s_ShaderPath + L"PSPixelLight.cso");

		pso.SetVertexShader(s_ShaderPath + L"VSPixelLight.cso");

		std::unique_ptr<Topology> topo = std::make_unique<Topology>(
			D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
			D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST
			);

		pso.SetTopology(topo.get());

		AddStaticBind(std::make_unique<PipelineState>(gfx, pso));

		AddStaticBind(std::move(rootSig));

		AddStaticBind(std::move(topo));

		Assimp::Importer imp;

		auto pModel = imp.ReadFile("models\\suzanne.obj",
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices
		);

		const auto pMesh = pModel->mMeshes[0];

		Vertices vertices = {
			vertexLayout,
			pMesh->mNumVertices
		};

		for (std::uint32_t i = 0; i < pMesh->mNumVertices; i++) {
			vertices.AddVertexData(
				DirectX::XMFLOAT3{
					pMesh->mVertices[i].x * scale,
					pMesh->mVertices[i].y * scale,
					pMesh->mVertices[i].z * scale
				},
				*reinterpret_cast<DirectX::XMFLOAT3*>(&pMesh->mNormals[i])
			);
		}

		AddStaticBind(std::make_unique<VertexBuffer>(
			vertices)
		);

		std::vector<std::uint16_t> indices;
		indices.reserve(pMesh->mNumVertices * 3);

		for (std::uint32_t i = 0; i < pMesh->mNumFaces; i++) {
			const auto& face = pMesh->mFaces[i];

			indices.push_back(static_cast<std::uint16_t>(face.mIndices[0]));
			indices.push_back(static_cast<std::uint16_t>(face.mIndices[1]));
			indices.push_back(static_cast<std::uint16_t>(face.mIndices[2]));
		}

		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(std::move(indices)));

		AddStaticBind(std::make_unique<ConstantBuffer<LightData>>(
			3u, static_cast<std::uint32_t>(sizeof(LightData) / 4u),
			std::bind(&Light::GetLightData, App::GetLight()))
		);
	}

	struct PSMaterial {
		DirectX::XMFLOAT4 material;
		float specularIntensity;
		float specularPower;
	};

	PSMaterial matData = {
		material,
		0.6f,
		30.0f
	};

	AddBind(std::make_unique<ConstantBufferCBVStatic<PSMaterial>>(
		2u, &matData
		));

	AddBind(std::make_unique<ConstantBufferMat>(
		0u, 16u, std::bind(&Transform::GetTransformWithProjectionCM, &m_Transform)
		));

	AddBind(std::make_unique<ConstantBufferCBVDynamic<DirectX::XMMATRIX>>(
		1u, std::bind(&Transform::GetTransformCM, &m_Transform)
		));
}

void Model::Update(float deltaTime) noexcept {
	m_Transform = m_tobj.GetMomentum(deltaTime);
}
