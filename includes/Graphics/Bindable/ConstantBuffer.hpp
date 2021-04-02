#ifndef __CONSTANT_BUFFER_HPP__
#define __CONSTANT_BUFFER_HPP__
#include <Bindable.hpp>
#include <memory>

template<typename T>
class ConstantBuffer : public Bindable {
public:
	ConstantBuffer()
		: m_Index(0u), m_ElementsNum(0u), m_Data(T()) {}

	ConstantBuffer(std::uint32_t indexInRootSignature,
		std::uint32_t elementsNumber, T&& data)
		:
		m_Index(indexInRootSignature), m_ElementsNum(elementsNumber),
		m_Data(std::move(data)) {}

	void BindCommand(Graphics& gfx) noexcept override {
		GetCommandList(gfx)->SetGraphicsRoot32BitConstants(
			m_Index,
			m_ElementsNum,
			&m_Data, 0
		);
	}

	void Update(
		std::uint32_t indexInRootSignature, std::uint32_t elementsNumber, T&& data
	) {
		m_Index = indexInRootSignature;
		m_ElementsNum = elementsNumber;

		m_Data = std::move(data);
	}

private:
	T m_Data;
	std::uint32_t m_Index;
	std::uint32_t m_ElementsNum;
};

#endif
