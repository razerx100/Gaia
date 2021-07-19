#include <BindableProcessor.hpp>
#include <Vertices.hpp>
#include <assimp/scene.h>
#include <PSODesc.hpp>
#include <BindAll.hpp>
#include <Graphics.hpp>
#include <Texture.hpp>
#include <Surface.hpp>
#include <Utilities.hpp>

BindProcessor::BindProcessor(const std::string& filePath, const aiMesh& mesh,
	const aiMaterial* const* pMaterials) {
	Init(filePath, mesh, pMaterials);
}

BindProcessor::BindProcessor(
	const std::string& objectName,
	LegacyType type,
	const std::string& texturePath
) {
	Init(objectName, type, texturePath);
}

void BindProcessor::Init(
	const std::string& filePath, const struct aiMesh& mesh,
	const struct aiMaterial* const* pMaterials
) {
	m_hasTexture = true;

	std::string objectName;
	m_texturePath = GUtil::GetFileRootFromPath(filePath);

	auto& material = *pMaterials[mesh.mMaterialIndex];

	aiString texFileName;

	material.GetTexture(aiTextureType_DIFFUSE, 0, &texFileName);
	objectName.append(texFileName.C_Str());

	m_hasSpecular = (material.GetTexture(aiTextureType_SPECULAR, 0, &texFileName)
		== aiReturn_SUCCESS);

	m_VShaderName = "VSPixelLight";

	if (m_hasSpecular) {
		m_rootSignature.first = "RSPixelLightSpec";
		m_PShaderName = "PSPixelLightSpec";
		objectName.append(std::string("#") + texFileName.C_Str());
	}
	else {
		m_rootSignature.first = "RSPixelLight";
		m_PShaderName = "PSPixelLight";
	}

	m_textures.first.append("Tex#" + objectName);

	m_vertexLayout = {
		{"Position", 12u},
		{"Normal", 12u},
		{"TexCoord", 8u}
	};

	m_topology.first = "TriangleList";

	m_pipelineState.first = GeneratePSOName(
		m_vertexLayout.GetTag(),
		m_rootSignature.first,
		m_VShaderName,
		m_PShaderName,
		m_topology.first
	);

	m_vertexBuffer.first = "VB" + objectName;
	m_indexBuffer.first = "IB" + objectName;
}
void BindProcessor::Init(
	const std::string& objectName,
	LegacyType type,
	const std::string& texturePath
) {
	m_legacyType = type;

	if (type == LegacyType::SolidColor) {
		m_vertexLayout = {
			{"Position", 12u},
			{"Normal", 12u}
		};

		m_VShaderName = "VSSolidColorNormal";
		m_PShaderName = "PSSolidColorNormal";
		m_rootSignature.first = "RSSolidColorNormal";
	}
	else if (type == LegacyType::WithTexture) {
		m_vertexLayout = {
			{"Position", 12u},
			{"Normal", 12u},
			{"TexCoord", 8u}
		};

		m_VShaderName = "VSPixelLight";
		m_PShaderName = "PSPixelLight";
		m_rootSignature.first = "RSPixelLight";

		m_textures.first.append("Tex#" + GUtil::GetNameFromPath(texturePath));
		m_texturePath = texturePath;
	}
	else if (type == LegacyType::SolidColorNoNorm) {
		m_vertexLayout = {
			{"Position", 12u}
		};

		m_VShaderName = "VSSolidColor";
		m_PShaderName = "PSSolidColor";
		m_rootSignature.first = "RSSolidColor";
	}

	m_topology.first = "TriangleList";

	m_pipelineState.first = GeneratePSOName(
		m_vertexLayout.GetTag(),
		m_rootSignature.first,
		m_VShaderName,
		m_PShaderName,
		m_topology.first
	);

	m_vertexBuffer.first = "VB" + objectName;
	m_indexBuffer.first = "IB" + objectName;
}

void BindProcessor::Process(
	Graphics& gfx,
	const struct aiMesh& mesh,
	const struct aiMaterial* const* pMaterials,
	std::uint32_t textureRootIndex
) {

	PSODesc pso;

	// Topology
	SetTopology(pso);

	// Root Signature
	SetRootSignature(gfx, pso);

	// PipelineState
	SetPipelineState(gfx, pso);

	// Vertex Buffer
	if (Codex::IsInCodex(m_vertexBuffer.first))
		m_vertexBuffer.second = Codex::GetBindableRef(m_vertexBuffer.first);
	else {
		Vertices vertices = {
			m_vertexLayout,
			mesh.mNumVertices
		};

		for (std::uint32_t i = 0; i < mesh.mNumVertices; ++i) {
			vertices.AddVertexData(
				*CastTo<DirectX::XMFLOAT3>(&mesh.mVertices[i]),
				*CastTo<DirectX::XMFLOAT3>(&mesh.mNormals[i]),
				*CastTo<DirectX::XMFLOAT2>(&mesh.mTextureCoords[0][i])
			);
		}

		m_vertexBuffer.second = Codex::AddAndGetBind(
			m_vertexBuffer.first,
			std::make_unique<VertexBuffer>(vertices)
		);
	}

	// Index Buffer
	if (Codex::IsInCodex(m_indexBuffer.first))
		m_indexBuffer.second = Codex::GetBindableRef(m_indexBuffer.first);
	else {
		std::vector<std::uint16_t> indices;
		indices.reserve(static_cast<std::uint64_t>(mesh.mNumFaces) * 3u);

		for (std::uint32_t i = 0; i < mesh.mNumFaces; ++i) {
			auto& face = mesh.mFaces[i];
			indices.emplace_back(static_cast<std::uint16_t>(face.mIndices[0]));
			indices.emplace_back(static_cast<std::uint16_t>(face.mIndices[1]));
			indices.emplace_back(static_cast<std::uint16_t>(face.mIndices[2]));
		}

		m_indexBuffer.second = Codex::AddAndGetBind(
			m_indexBuffer.first,
			std::make_unique<IndexBuffer>(indices)
		);
	}

	// Textures
	if (Codex::IsInCodex(m_textures.first))
		m_textures.second = Codex::GetBindableRef(m_textures.first);
	else {
		auto& material = *pMaterials[mesh.mMaterialIndex];
		aiString texFileName;

		auto textureHeap = std::make_unique<BindableHeap>(gfx, textureRootIndex);

		material.GetTexture(aiTextureType_DIFFUSE, 0, &texFileName);
		textureHeap->AddSRV(gfx, std::make_unique<Texture>(
			gfx,
			Surface::FromFile(
				m_texturePath + texFileName.C_Str()
			),
			0u
			)
		);

		if (m_hasSpecular) {
			material.GetTexture(aiTextureType_SPECULAR, 0, &texFileName);

			textureHeap->AddSRV(gfx, std::make_unique<Texture>(
				gfx,
				Surface::FromFile(
					m_texturePath + texFileName.C_Str()
				),
				1u
				)
			);
		}
		else
			material.Get(AI_MATKEY_SHININESS, m_shininess);

		textureHeap->FinishAdding(gfx);

		m_textures.second = Codex::AddAndGetBind(
			m_textures.first,
			std::move(textureHeap)
		);
	}
}

void BindProcessor::ProcessLegacyType(
	Graphics& gfx,
	const IndexedTriangleList& modelData,
	std::uint32_t textureRootIndex
) {
	PSODesc pso;

	// Topology
	SetTopology(pso);

	// Root Signature
	SetRootSignature(gfx, pso);

	// PipelineState
	SetPipelineState(gfx, pso);

	// Vertex Buffer
	if (Codex::IsInCodex(m_vertexBuffer.first))
		m_vertexBuffer.second = Codex::GetBindableRef(m_vertexBuffer.first);
	else
		m_vertexBuffer.second = Codex::AddAndGetBind(
			m_vertexBuffer.first,
			std::make_unique<VertexBuffer>(
				modelData.GetVerticesObject(m_vertexLayout, m_legacyType)
				)
		);

	// Index Buffer
	if (Codex::IsInCodex(m_indexBuffer.first))
		m_indexBuffer.second = Codex::GetBindableRef(m_indexBuffer.first);
	else
		m_indexBuffer.second = Codex::AddAndGetBind(
			m_indexBuffer.first,
			std::make_unique<IndexBuffer>(modelData.GetIndices())
		);

	// Texture
	if (m_legacyType == LegacyType::WithTexture) {
		if (Codex::IsInCodex(m_textures.first))
			m_textures.second = Codex::GetBindableRef(m_textures.first);
		else {
			auto textureHeap = std::make_unique<BindableHeap>(gfx, textureRootIndex);

			textureHeap->AddSRV(gfx, std::make_unique<Texture>(
				gfx,
				Surface::FromFile(
					m_texturePath
				),
				0u
				)
			);

			textureHeap->FinishAdding(gfx);

			m_textures.second = Codex::AddAndGetBind(
				m_textures.first,
				std::move(textureHeap)
			);
		}
	}
}

// Getters

bool BindProcessor::IsSpecularAvailable() const noexcept {
	return m_hasSpecular;
}

float BindProcessor::GetShininess() const noexcept {
	return m_shininess;
}

BindPtr* BindProcessor::GetTopology() const noexcept {
	return m_topology.second;
}

BindPtr* BindProcessor::GetRootSignature() const noexcept {
	return m_rootSignature.second;
}

BindPtr* BindProcessor::GetPipelineState() const noexcept {
	return m_pipelineState.second;
}

BindPtr* BindProcessor::GetVertexBuffer() const noexcept {
	return m_vertexBuffer.second;
}

BindPtr* BindProcessor::GetIndexBuffer() const noexcept {
	return m_indexBuffer.second;
}
BindPtr* BindProcessor::GetTextures() const noexcept {
	return m_textures.second;
}

const std::string& BindProcessor::GetTexturesName() const noexcept {
	return m_textures.first;
}

// Setters

void BindProcessor::SetTopology(PSODesc& pso) {
	if (Codex::IsInCodex(m_topology.first)) {
		BindPtr* topology = Codex::GetBindableRef(m_topology.first);
		pso.SetTopology(CastTo<Topology>(topology->m_pBind.get()));
		m_topology.second = topology;
	}
	else {
		auto topology = std::make_unique<Topology>(m_topology.first);
		pso.SetTopology(topology.get());

		m_topology.second = Codex::AddAndGetBind(
			m_topology.first,
			std::move(topology)
		);
	}
}

void BindProcessor::SetRootSignature(Graphics& gfx, PSODesc& pso) {
	if (Codex::IsInCodex(m_rootSignature.first)) {
		BindPtr* rootSignature = Codex::GetBindableRef(m_rootSignature.first);
		pso.SetRootSignature(CastTo<RootSignature>(rootSignature->m_pBind.get()));
		m_rootSignature.second = rootSignature;
	}
	else {
		/* // Logic of Dynamic needs to be updated before using
		auto rootSigDy = std::make_unique<RootSignatureDynamic>();

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

		auto rootSignature = std::make_unique<RootSignaturePreCompiled>(
			gfx,
			GUtil::ShaderPath::Get()
			+ m_rootSignature.first + ".cso"
			);
		pso.SetRootSignature(rootSignature.get());

		m_rootSignature.second = Codex::AddAndGetBind(
			m_rootSignature.first,
			std::move(rootSignature)
		);
	}
}

void BindProcessor::SetPipelineState(Graphics& gfx, PSODesc& pso) {
	if (Codex::IsInCodex(m_pipelineState.first))
		m_pipelineState.second = Codex::GetBindableRef(m_pipelineState.first);
	else {
		pso.SetInputLayout(m_vertexLayout);
		pso.SetVertexShader(
			GUtil::ShaderPath::Get()
			+ m_VShaderName + ".cso"
		);
		pso.SetPixelShader(
			GUtil::ShaderPath::Get()
			+ m_PShaderName + ".cso"
		);

		m_pipelineState.second = Codex::AddAndGetBind(
			m_pipelineState.first,
			std::make_unique<PipelineState>(
				gfx,
				pso
				)
		);
	}
}
