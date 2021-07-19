#include <Model.hpp>
#include <Graphics.hpp>
#include <Mesh.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <BindAll.hpp>
#include <Texture.hpp>
#include <Surface.hpp>
#include <BindableCodex.hpp>
#include <InputLayout.hpp>
#include <Vertices.hpp>
#include <PSODesc.hpp>
#include <BindableProcessor.hpp>

Model::Model(
	Graphics& gfx, const std::string& objectName, const std::string& fileName
) : m_name(objectName) {
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
			pScene->mMaterials,
			fileName
			)
		);

	m_pRoot = ParseNode(*pScene->mRootNode);
}

std::unique_ptr<Mesh> Model::ParseMesh(
	Graphics& gfx, const aiMesh& mesh,
	const aiMaterial* const* pMaterials,
	const std::string& filePath
) {
	BindProcessor process = BindProcessor(
		filePath,
		mesh,
		pMaterials
	);

	process.Process(gfx, mesh, pMaterials, 3u);

	std::vector<std::unique_ptr<Bindable>> perInstanceBInd;

	if (!process.IsSpecularAvailable()) {
		struct PSMaterial {
			float specularIntensity;
			float specularPower;
		};

		PSMaterial matData = {
			0.8f,
			process.GetShininess()
		};

		perInstanceBInd.emplace_back(
			std::make_unique<ConstantBufferCBVStatic<PSMaterial>>(
				4u, &matData
				)
		);
	}

	std::vector<BindPtr*> bindables;

	bindables.emplace_back(process.GetPipelineState());
	bindables.emplace_back(process.GetRootSignature());
	bindables.emplace_back(process.GetTopology());
	bindables.emplace_back(process.GetVertexBuffer());
	bindables.emplace_back(process.GetIndexBuffer());
	bindables.emplace_back(process.GetTextures());

	// Constant buffers
	// Root Index Slot 1 is reserved for Light Buffer data from Light object

	auto pMesh = std::make_unique<Mesh>(
			std::move(bindables),
			process.GetTexturesName()
		);

	perInstanceBInd.emplace_back(
		std::make_unique<ConstantBufferMat>(
			0u, 16u, std::bind(
				&Transform::GetTransformWithProjectionCM,
				pMesh->GetTransform())
			)
	);

	perInstanceBInd.emplace_back(
		std::make_unique<ConstantBufferCBVDynamic<DirectX::XMMATRIX>>(
			2u, std::bind(&Transform::GetTransformCM, pMesh->GetTransform())
			)
	);

	pMesh->AddBinds(std::move(perInstanceBInd));

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

const std::string& Model::GetName() const noexcept {
	return m_name;
}

// Node

Node::Node(std::vector<Mesh*>&& pMeshes, const DirectX::XMMATRIX& transform)
	: m_pMeshes(std::move(pMeshes)), m_transform(transform) {}

void Node::Draw(
	Graphics& gfx,
	const DirectX::XMMATRIX& accumulatedTransform
) const noexcept {
	DirectX::XMMATRIX accumulationNow = m_transform * accumulatedTransform;

	for (Mesh* pMesh : m_pMeshes)
		pMesh->Draw(gfx, accumulationNow);

	for (auto& pChild : m_pChildren)
		pChild->Draw(gfx, accumulationNow);
}

void Node::AddChild(std::unique_ptr<Node> pChild) noexcept {
	m_pChildren.emplace_back(std::move(pChild));
}
