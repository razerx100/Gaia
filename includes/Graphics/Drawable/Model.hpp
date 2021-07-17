#ifndef __MODEL_HPP__
#define __MODEL_HPP__
#include <vector>
#include <memory>
#include <string>
#include <DirectXMath.h>
#include <Mesh.hpp>

class Node;
class Graphics;

class Model {
public:
	Model(Graphics& gfx, const std::string& objectName, const std::string& fileName);

	std::unique_ptr<Mesh> ParseMesh(
		Graphics& gfx, const struct aiMesh& mesh,
		const struct aiMaterial* const* pMaterials,
		const std::string& filePath
	);
	std::unique_ptr<Node> ParseNode(const struct aiNode& node);

	void Draw(Graphics& gfx, const DirectX::XMMATRIX& transform) const;

	const std::string& GetName() const noexcept;

private:
	std::unique_ptr<Node> m_pRoot;
	std::vector<std::unique_ptr<Mesh>> m_pMeshes;
	std::string m_name;
};

class Node {
	friend class Model;
public:
	Node(std::vector<Mesh*>&& pMeshes, const DirectX::XMMATRIX& transform);

	void Draw(
		Graphics& gfx,
		const DirectX::XMMATRIX& accumulatedTransform
	) const noexcept;

private:
	void AddChild(std::unique_ptr<Node> pChild) noexcept;

private:
	std::vector<std::unique_ptr<Node>> m_pChildren;
	std::vector<Mesh*> m_pMeshes;
	DirectX::XMMATRIX m_transform;
};
#endif
