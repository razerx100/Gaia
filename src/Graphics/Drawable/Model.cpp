#include <Model.hpp>
#include <Graphics.hpp>
#include <Mesh.hpp>
#include <App.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <BindAll.hpp>
#include <Texture.hpp>
#include <Surface.hpp>

Model::Model(Graphics& gfx, const std::string& fileName) {
	Assimp::Importer importer;

	const auto pScene = importer.ReadFile(
		fileName.c_str(),
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ConvertToLeftHanded |
		aiProcess_GenNormals
	);

	for (std::uint32_t i = 0; i < pScene->mNumMeshes; ++i)
		m_pMeshes.emplace_back(ParseMesh(
			gfx, *pScene->mMeshes[i],
			pScene->mMaterials
			)
		);

	m_pRoot = ParseNode(*pScene->mRootNode);
}

std::unique_ptr<Mesh> Model::ParseMesh(
	Graphics& gfx, const aiMesh& mesh,
	const struct aiMaterial* const* pMaterials
) {
	VertexLayout vertexLayout = {
		{"Position", 12u},
		{"Normal", 12u},
		{"TexCoord", 8u}
	};

	Vertices vertices = {
		vertexLayout,
		mesh.mNumVertices
	};

	std::deque<std::unique_ptr<Bindable>> bindables;

	bool hasSpecular = false;
	float shininess = 35.0f;

	if (mesh.mMaterialIndex >= 0) {
		auto& material = *pMaterials[mesh.mMaterialIndex];
		aiString texFileName;

		auto textureHeap = std::make_unique<BindableHeap>(gfx, 3u);

		std::string modelPath = "models\\nano_textured\\";

		material.GetTexture(aiTextureType_DIFFUSE, 0, &texFileName);
		textureHeap->AddSRV(gfx, std::make_unique<Texture>(
			gfx,
			Surface::FromFile(
				modelPath + texFileName.C_Str()
			),
			0u
			)
		);

		if (material.GetTexture(aiTextureType_SPECULAR, 0, &texFileName)
			== aiReturn_SUCCESS) {
			textureHeap->AddSRV(gfx, std::make_unique<Texture>(
				gfx,
				Surface::FromFile(
					modelPath + texFileName.C_Str()
				),
				1u
				)
			);

			hasSpecular = true;
		}
		else
			material.Get(AI_MATKEY_SHININESS, shininess);

		textureHeap->FinishAdding(gfx);
		bindables.emplace_back(std::move(textureHeap));
	}

	for (std::uint32_t i = 0; i < mesh.mNumVertices; ++i) {
		vertices.AddVertexData(
			*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mVertices[i]),
			*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mNormals[i]),
			*reinterpret_cast<DirectX::XMFLOAT2*>(&mesh.mTextureCoords[0][i])
		);
	}

	std::vector<std::uint16_t> indices;
	indices.reserve(mesh.mNumFaces * 3);

	for (std::uint32_t i = 0; i < mesh.mNumFaces; ++i) {
		auto& face = mesh.mFaces[i];
		indices.emplace_back(static_cast<std::uint16_t>(face.mIndices[0]));
		indices.emplace_back(static_cast<std::uint16_t>(face.mIndices[1]));
		indices.emplace_back(static_cast<std::uint16_t>(face.mIndices[2]));
	}

	bindables.emplace_back(std::make_unique<VertexBuffer>(vertices));

	PSODesc pso = PSODesc();

	pso.SetInputLayout(vertexLayout);

	pso.SetVertexShader(App::GetShaderPath() + L"VSPixelLight.cso");

	std::unique_ptr<RootSignature> rootSig;
	if (hasSpecular) {
		rootSig = std::make_unique<RootSignaturePreCompiled>(
			gfx, App::GetShaderPath() + L"RSPixelLightSpec.cso"
			);

		pso.SetPixelShader(App::GetShaderPath() + L"PSPixelLightSpec.cso");
	}
	else {
		rootSig = std::make_unique<RootSignaturePreCompiled>(
			gfx, App::GetShaderPath() + L"RSPixelLight.cso"
			);

		/*auto rootSigDy = std::make_unique<RootSignatureDynamic>();

		rootSigDy->AddConstants(16u, D3D12_SHADER_VISIBILITY_VERTEX, 0u);
		rootSigDy->AddConstantBufferView(D3D12_SHADER_VISIBILITY_VERTEX, 1u);
		rootSigDy->AddConstants(15u, D3D12_SHADER_VISIBILITY_PIXEL, 1u, 1u);
		rootSigDy->AddDescriptorTable(
			D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
			1u,
			D3D12_SHADER_VISIBILITY_PIXEL,
			0u
		);
		rootSigDy->AddConstantBufferView(D3D12_SHADER_VISIBILITY_PIXEL, 0u, 1u);
		rootSigDy->AddStaticSampler();

		rootSigDy->CompileSignature(gfx);

		rootSig = std::move(rootSigDy);*/

		pso.SetPixelShader(App::GetShaderPath() + L"PSPixelLight.cso");

		struct PSMaterial {
			float specularIntensity;
			float specularPower;
		};

		PSMaterial matData = {
			0.8f,
			shininess
		};

		bindables.emplace_back(std::make_unique<ConstantBufferCBVStatic<PSMaterial>>(
			4u, &matData
			)
		);
	}

	pso.SetRootSignature(rootSig.get());

	std::unique_ptr<Topology> topo = std::make_unique<Topology>(
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST
		);

	pso.SetTopology(topo.get());

	bindables.emplace_front(std::make_unique<PipelineState>(gfx, pso));

	bindables.emplace_front(std::move(rootSig));

	bindables.emplace_back(std::move(topo));

	// Constant buffers

	bindables.emplace_back(std::make_unique<ConstantBuffer<LightData>>(
		2u, static_cast<std::uint32_t>(sizeof(LightData) / 4u),
		std::bind(&Light::GetLightData, App::GetLight())
		)
	);

	auto pMesh = std::make_unique<Mesh>(
		std::move(bindables),
		std::make_unique<IndexBuffer>(indices)
		);

	pMesh->AddBind(std::make_unique<ConstantBufferMat>(
		0u, 16u, std::bind(
			&Transform::GetTransformWithProjectionCM,
			pMesh->GetTransform())
		)
	);

	pMesh->AddBind(std::make_unique<ConstantBufferCBVDynamic<DirectX::XMMATRIX>>(
		1u, std::bind(&Transform::GetTransformCM, pMesh->GetTransform())
		)
	);

	return pMesh;
}

std::unique_ptr<Node> Model::ParseNode(const aiNode& node) {
	DirectX::XMMATRIX transform = DirectX::XMMatrixTranspose(
		DirectX::XMLoadFloat4x4(
			reinterpret_cast<const DirectX::XMFLOAT4X4*>(
				&node.mTransformation
				)
		)
	);

	std::vector<Mesh*> pMeshes;
	pMeshes.reserve(node.mNumMeshes);

	for (std::uint32_t i = 0; i < node.mNumMeshes; ++i) {
		const auto meshIndex = node.mMeshes[i];

		pMeshes.emplace_back(m_pMeshes[meshIndex].get());
	}

	auto pNode = std::make_unique<Node>(std::move(pMeshes), transform);

	for (std::uint32_t i = 0; i < node.mNumChildren; ++i)
		pNode->AddChild(ParseNode(*node.mChildren[i]));

	return pNode;
}

void Model::Draw(Graphics& gfx, const DirectX::XMMATRIX& transform) const {
	m_pRoot->Draw(gfx, transform);
}

Node::Node(std::vector<Mesh*>&& pMeshes, const DirectX::XMMATRIX& transform)
	: m_pMeshes(std::move(pMeshes)), m_Transform(transform) {}

void Node::Draw(
	Graphics& gfx,
	const DirectX::XMMATRIX& accumulatedTransform
) const noexcept {
	DirectX::XMMATRIX accumulationNow = m_Transform * accumulatedTransform;

	for (Mesh* pMesh : m_pMeshes)
		pMesh->Draw(gfx, accumulationNow);

	for (auto& pChild : m_pChildren)
		pChild->Draw(gfx, accumulationNow);
}

void Node::AddChild(std::unique_ptr<Node> pChild) noexcept {
	m_pChildren.emplace_back(std::move(pChild));
}
