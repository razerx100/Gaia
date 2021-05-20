#ifndef __CONSTANT_BUFFER_HPP__
#define __CONSTANT_BUFFER_HPP__
#include <Bindable.hpp>
#include <Drawable.hpp>
#include <memory>
#include <functional>
#include <GraphicsThrowMacros.hpp>
#include <d3dx12.h>
#include <BufferMan.hpp>

template<typename T>
class ConstantBuffer : public Bindable {
public:
	ConstantBuffer(std::uint32_t indexInRootSignature,
		std::uint32_t elementsNumber, std::function<T()> getter
		) : m_RootIndex(indexInRootSignature), m_ElementsNumber(elementsNumber),
		m_Getter(getter) {}

	void BindCommand(Graphics& gfx) noexcept override {
		T constData = m_Getter();

		GetCommandList(gfx)->SetGraphicsRoot32BitConstants(
			m_RootIndex,
			m_ElementsNumber,
			&constData,
			0
		);
	}

private:
	std::uint32_t m_RootIndex;
	std::uint32_t m_ElementsNumber;
	std::function<T()> m_Getter;
};

template<typename T>
class _ConstantBufferRV : public Bindable {
public:
	_ConstantBufferRV(
		std::uint32_t indexInRootSignature,
		std::uint32_t bufferSize, std::uint8_t** cpuPtr
	)
		: m_RootIndex(indexInRootSignature) {
		m_pBuffer = BufferMan::RequestMemory(bufferSize, 256u);
		*cpuPtr = m_pBuffer->cpuPTR;
	}

protected:
	std::uint32_t m_RootIndex;
	std::unique_ptr<Memory> m_pBuffer;
};

template<typename T>
class ConstantBufferCBV : public _ConstantBufferRV<T> {
public:
	using _ConstantBufferRV<T>::_ConstantBufferRV;

	void BindCommand(Graphics& gfx) noexcept override {
		Bindable::GetCommandList(gfx)->SetGraphicsRootConstantBufferView(
			_ConstantBufferRV<T>::m_RootIndex, _ConstantBufferRV<T>::m_pBuffer->gpuPTR
		);
	}
};

typedef ConstantBuffer<DirectX::XMMATRIX> ConstantBufferMat;
#endif
