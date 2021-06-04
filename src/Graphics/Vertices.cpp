#include <Vertices.hpp>

Vertices::Vertices(const VertexLayout& vertexLayout, std::uint32_t elementsNumber)
    :
    m_bufferSizeByte(elementsNumber * vertexLayout.SizeByte()),
    m_elementsNumber(elementsNumber),
    m_buffer(new std::uint8_t[m_bufferSizeByte]),
    m_layout(vertexLayout),
    m_currentElement(0u),
    m_offset(0) {}

std::uint8_t* Vertices::GetBuffer() const noexcept {
    return m_buffer.get();
}

std::uint32_t Vertices::GetSizeByte() const noexcept {
    return m_bufferSizeByte;
}

std::uint32_t Vertices::GetElementsNumber() const noexcept {
    return m_elementsNumber;
}

std::uint32_t Vertices::GetVertexSizeByte() const noexcept {
    return m_layout.SizeByte();
}
