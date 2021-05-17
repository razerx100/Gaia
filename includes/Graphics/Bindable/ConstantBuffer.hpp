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
protected:
	struct {
		std::uint32_t elements;
		std::function<T()> getter;
	} m_ConstantInfo;

public:
	ConstantBuffer(std::uint32_t indexInRootSignature,
		std::uint32_t elementsNumber, std::function<T()> getter)
		: m_RSIndex(indexInRootSignature), m_ConstantInfo{elementsNumber, getter} {}

	ConstantBuffer(std::uint32_t indexInRootSignature)
		: m_RSIndex(indexInRootSignature) {}

	void BindCommand(Graphics& gfx) noexcept override {
		T constData = m_ConstantInfo.getter();

		GetCommandList(gfx)->SetGraphicsRoot32BitConstants(
			m_RSIndex,
			m_ConstantInfo.elements,
			&constData, 0
		);
	}

protected:
	std::uint32_t m_RSIndex;

	std::unique_ptr<Memory> m_pBuffer;
};

template<typename T>
class PixelConstantBuffer : public ConstantBuffer<T> {
public:
	PixelConstantBuffer(
		std::uint32_t indexInRootSignature,
		std::uint32_t bufferSize, std::uint8_t** cpuPtr
	)
		: ConstantBuffer<T>::ConstantBuffer(indexInRootSignature) {

		ConstantBuffer<T>::m_pBuffer = BufferMan::RequestMemory(bufferSize, 256u);
		*cpuPtr = ConstantBuffer<T>::m_pBuffer->cpuPTR;
	}

	void BindCommand(Graphics& gfx) noexcept override {
		Bindable::GetCommandList(gfx)->SetGraphicsRootConstantBufferView(
			ConstantBuffer<T>::m_RSIndex, ConstantBuffer<T>::m_pBuffer->gpuPTR
		);
	}
};

class VertexConstantBuffer : public ConstantBuffer<DirectX::XMMATRIX> {
public:
	using ConstantBuffer<DirectX::XMMATRIX>::ConstantBuffer;

	void BindCommand(Graphics& gfx) noexcept override;
};

#endif
