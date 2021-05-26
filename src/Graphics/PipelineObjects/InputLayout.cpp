#include <InputLayout.hpp>

InputLayout::InputLayout(std::vector<D3D12_INPUT_ELEMENT_DESC>&& inputLayout)
	: m_InputLayout{} {

	m_InputLayout.pInputElementDescs = inputLayout.data();
	m_InputLayout.NumElements = static_cast<std::uint32_t>(std::size(inputLayout));
}

void InputLayout::Init(std::vector<D3D12_INPUT_ELEMENT_DESC>&& inputLayout) {
	m_InputLayout.pInputElementDescs = inputLayout.data();
	m_InputLayout.NumElements = static_cast<std::uint32_t>(std::size(inputLayout));
}

D3D12_INPUT_LAYOUT_DESC InputLayout::GetInputLayout() const noexcept {
	return m_InputLayout;
}
