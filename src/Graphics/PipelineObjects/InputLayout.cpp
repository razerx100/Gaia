#include <InputLayout.hpp>

std::vector<DXGI_FORMAT> InputLayout::s_formats = {
	DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN,
	DXGI_FORMAT_R32_FLOAT,
	DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN,
	DXGI_FORMAT_R32G32_FLOAT,
	DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN,
	DXGI_FORMAT_R32G32B32_FLOAT,
	DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN,
	DXGI_FORMAT_R32G32B32A32_FLOAT
};


void InputLayout::Init(const VertexLayout& inputLayout) {
	m_elementNames.reserve(inputLayout.GetElements().size());

	for (std::uint32_t offset = 0; VertexData& vertex : inputLayout.GetElements()) {
		m_elementNames.emplace_back(vertex.name);

		D3D12_INPUT_ELEMENT_DESC element = {
			m_elementNames.back().c_str(), 0, s_formats[vertex.size],
			0, offset, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		};

		m_InputDescs.emplace_back(element);
		offset += vertex.size;
	}

	m_InputLayout.pInputElementDescs = m_InputDescs.data();
	m_InputLayout.NumElements = static_cast<std::uint32_t>(std::size(m_InputDescs));
}

D3D12_INPUT_LAYOUT_DESC InputLayout::GetInputLayout() const noexcept {
	return m_InputLayout;
}

VertexLayout::VertexLayout(std::initializer_list<VertexData> list)
	: m_sizeByte(0u) {
	for (VertexData element : list) {
		m_sizeByte += element.size;
		m_elements.emplace_back(element);
	}
}

VertexLayout::VertexLayout(const VertexLayout& vertex)
	: m_sizeByte(vertex.m_sizeByte) {
	for (VertexData element : vertex.m_elements)
		m_elements.emplace_back(element);
}

std::uint32_t VertexLayout::SizeByte() const noexcept {
	return m_sizeByte;
}

std::uint32_t VertexLayout::ElementsNumber() const noexcept {
	return static_cast<std::uint32_t>(m_elements.size());
}

std::uint32_t VertexLayout::GetElementSizeByte(std::uint32_t index) const noexcept {
	return m_elements[index].size;
}

std::vector<VertexData> VertexLayout::GetElements() const noexcept {
	return m_elements;
}
