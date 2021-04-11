#ifndef __CONSTANT_BUFFER_HPP__
#define __CONSTANT_BUFFER_HPP__
#include <Bindable.hpp>
#include <Drawable.hpp>
#include <memory>

template<typename T>
class ConstantBuffer : public Bindable {
public:
	ConstantBuffer(std::uint32_t indexInRootSignature,
		std::uint32_t elementsNumber, Drawable& parent)
		:
		m_Index(indexInRootSignature), m_ElementsNum(elementsNumber),
		m_ParentRef(parent) {}

	void BindCommand(Graphics& gfx) noexcept override {
		GetCommandList(gfx)->SetGraphicsRoot32BitConstants(
			m_Index,
			m_ElementsNum,
			&s_Data, 0
		);
	}

	static void SetData(T&& data) {
		s_Data = std::move(data);
	}

protected:
	std::uint32_t m_Index;
	std::uint32_t m_ElementsNum;
	Drawable& m_ParentRef;

private:
	static T s_Data;
};

template<typename T>
T ConstantBuffer<T>::s_Data;

class VertexConstantBuffer : public ConstantBuffer<DirectX::XMMATRIX> {
public:
	using ConstantBuffer<DirectX::XMMATRIX>::ConstantBuffer;

	void BindCommand(Graphics& gfx) noexcept override;
};

#endif
