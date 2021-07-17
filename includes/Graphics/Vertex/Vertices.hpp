#ifndef __VERTICES_HPP__
#define __VERTICES_HPP__
#include <InputLayout.hpp>
#include <memory>

class Vertices {
public:
    Vertices() = default;
    Vertices(const VertexLayout& vertexLayout, std::uint32_t elementsNumber);

    void Init(const VertexLayout& vertexLayout, std::uint32_t elementsNumber);

    template<class T, class ... Ts>
    void AddVertexData(T&& arg, Ts&& ... args) noexcept {
        memcpy(m_buffer.get() + m_offset, &arg,
            m_layout.GetElementSizeByte(m_currentElement));

        m_offset += m_layout.GetElementSizeByte(m_currentElement);
        m_currentElement++;

        AddVertexData(std::forward<Ts>(args)...);
    }

    template<class T>
    void AddVertexData(T&& arg) noexcept {
        memcpy(m_buffer.get() + m_offset, &arg,
            m_layout.GetElementSizeByte(m_currentElement));

        m_offset += m_layout.GetElementSizeByte(m_currentElement);
        m_currentElement = 0u;
    }

    std::uint8_t* GetBuffer() const noexcept;
    std::uint32_t GetSizeByte() const noexcept;
    std::uint32_t GetElementsNumber() const noexcept;
    std::uint32_t GetVertexSizeByte() const noexcept;

private:
    std::uint32_t m_bufferSizeByte;
    std::uint32_t m_elementsNumber;
    std::unique_ptr<std::uint8_t> m_buffer;

    VertexLayout m_layout;
    std::uint32_t m_currentElement;
    std::uint32_t m_offset;
};
#endif
