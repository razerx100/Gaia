#ifndef __VERTEX_LAYOUT_HPP__
#define __VERTEX_LAYOUT_HPP__
#include <Bindable.hpp>
#include <vector>

class VertexLayout : public Bindable {
public:
	VertexLayout(
		Graphics& gfx, const std::vector<D3D11_INPUT_ELEMENT_DESC>& inputDescs,
		ID3DBlob* vertexShaderByteCode
	);

	void Bind(Graphics& gfx) noexcept override;

private:
	ComPtr<ID3D11InputLayout> m_pInputLayout;
};
#endif
