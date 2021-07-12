#ifndef __INPUT_LAYOUT_HPP__
#define __INPUT_LAYOUT_HPP__
#include <CleanWin.hpp>
#include <d3d12.h>
#include <vector>
#include <string>

struct VertexData {
    std::string name;
    std::uint32_t size;
};

class VertexLayout {
public:
    VertexLayout() = default;
    VertexLayout(std::initializer_list<VertexData> list);
    VertexLayout(const VertexLayout& vertex);

    VertexLayout& operator=(const VertexLayout& vertex) noexcept;

    std::uint32_t SizeByte() const noexcept;
    std::uint32_t ElementsNumber() const noexcept;
    std::uint32_t GetElementSizeByte(std::uint32_t index) const noexcept;
    std::vector<VertexData> GetElements() const noexcept;
    std::string GetTag() const noexcept;

private:
    std::vector<VertexData> m_elements;
    std::uint32_t m_sizeByte;
};

class InputLayout {
public:
	InputLayout() = default;

	void Init(const VertexLayout& inputLayout);
	D3D12_INPUT_LAYOUT_DESC GetInputLayout() const noexcept;

private:
	D3D12_INPUT_LAYOUT_DESC m_InputLayout;
	std::vector<D3D12_INPUT_ELEMENT_DESC> m_InputDescs;
    std::vector<std::string> m_elementNames;

	static std::vector<DXGI_FORMAT> s_formats;
};
#endif
