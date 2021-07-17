#ifndef __BINDABLE_PROCESSOR_HPP__
#define __BINDABLE_PROCESSOR_HPP__
#include <memory>
#include <utility>
#include <InputLayout.hpp>
#include <BindableCodex.hpp>
#include <IndexedTriangleList.hpp>

class Graphics;
class PSODesc;

class BindProcessor {
public:
	BindProcessor() = default;
	BindProcessor(
		const std::string& filePath, const struct aiMesh& mesh,
		const struct aiMaterial* const* pMaterials
	);
	BindProcessor(
		const std::string& objectName,
		LegacyType type,
		const std::string& texturePath = ""
	);

	BindProcessor(const BindProcessor&) = delete;
	BindProcessor(BindProcessor&&) = delete;

	void Init(
		const std::string& filePath, const struct aiMesh& mesh,
		const struct aiMaterial* const* pMaterials
	);
	void Init(
		const std::string& objectName,
		LegacyType type,
		const std::string& texturePath = ""
	);
	void Process(
		Graphics& gfx,
		const struct aiMesh& mesh,
		const struct aiMaterial* const* pMaterials,
		std::uint32_t textureRootIndex
	);
	void ProcessLegacyType(
		Graphics& gfx,
		const class IndexedTriangleList& modelData,
		std::uint32_t textureRootIndex = 0u
	);

	bool IsSpecularAvailable() const noexcept;
	float GetShininess() const noexcept;

	BindPtr* GetTopology() const noexcept;
	BindPtr* GetRootSignature() const noexcept;
	BindPtr* GetPipelineState() const noexcept;
	BindPtr* GetVertexBuffer() const noexcept;
	BindPtr* GetIndexBuffer() const noexcept;
	BindPtr* GetTextures() const noexcept;

	const std::string& GetTexturesName() const noexcept;

private:
	void SetTopology(PSODesc& pso);
	void SetRootSignature(Graphics& gfx, PSODesc& pso);
	void SetPipelineState(Graphics& gfx, PSODesc& pso);

private:
	template<typename C, typename B>
	C* CastTo(B* ptr) noexcept {
		return reinterpret_cast<C*>(ptr);
	}

public:
	template<typename T, typename ...Ts>
	static BindPtr* GetOrAddGeneric(const std::string& key, Ts&& ...args) {
		if (Codex::IsInCodex(key))
			return Codex::GetBindableRef(key);
		else {
			return Codex::AddAndGetBind(
				key,
				std::make_unique<T>(std::forward<Ts>(args)...)
			);
		}
	}

private:
	std::pair<std::string, BindPtr*> m_rootSignature;
	std::pair<std::string, BindPtr*> m_vertexBuffer;
	std::pair<std::string, BindPtr*> m_indexBuffer;
	std::pair<std::string, BindPtr*> m_topology;
	std::pair<std::string, BindPtr*> m_pipelineState;
	std::pair<std::string, BindPtr*> m_textures;

	std::string m_VShaderName;
	std::string m_PShaderName;

	VertexLayout m_vertexLayout;
	std::string m_texturePath;
	LegacyType m_legacyType;
	float m_shininess = 35.0f;

	bool m_hasTexture;
	bool m_hasSpecular;
};
#endif
