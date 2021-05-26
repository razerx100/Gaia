#ifndef __INPUT_LAYOUT_HPP__
#define __INPUT_LAYOUT_HPP__
#include <CleanWin.hpp>
#include <d3d12.h>
#include <vector>

class InputLayout {
public:
	InputLayout() = default;

	InputLayout(std::vector<D3D12_INPUT_ELEMENT_DESC>&& inputLayout);

	void Init(std::vector<D3D12_INPUT_ELEMENT_DESC>&& inputLayout);

	D3D12_INPUT_LAYOUT_DESC GetInputLayout() const noexcept;

private:
	D3D12_INPUT_LAYOUT_DESC m_InputLayout;
};
#endif
