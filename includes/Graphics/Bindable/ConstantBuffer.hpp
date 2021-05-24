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
class _ConstantBufferRVStatic : public Bindable {
public:
	_ConstantBufferRVStatic(
		std::uint32_t indexInRootSignature,
		T* staticData
	)
		: m_RootIndex(indexInRootSignature) {

		std::uint32_t bufferSize = static_cast<std::uint32_t>(sizeof(T));
		m_pBuffer = BufferMan::RequestMemory(bufferSize, 256u);

		memcpy(m_pBuffer->cpuPTR, staticData, bufferSize);
	}

protected:
	std::uint32_t m_RootIndex;
	std::unique_ptr<Memory> m_pBuffer;
};

template<typename T>
class _ConstantBufferRVDynamic : public Bindable {
public:
	_ConstantBufferRVDynamic(
		std::uint32_t indexInRootSignature,
		std::function<T()> getter
	)
		: m_RootIndex(indexInRootSignature), m_Getter(getter) {

		std::uint32_t bufferSize = static_cast<std::uint32_t>(sizeof(T));
		m_pBuffer = BufferMan::RequestMemory(bufferSize, 256u);
	}

protected:
	std::uint32_t m_RootIndex;
	std::unique_ptr<Memory> m_pBuffer;
	std::function<T()> m_Getter;
};

template<typename T>
class ConstantBufferCBVStatic : public _ConstantBufferRVStatic<T> {
public:
	using _ConstantBufferRVStatic<T>::_ConstantBufferRVStatic;

	void BindCommand(Graphics& gfx) noexcept override {
		Bindable::GetCommandList(gfx)->SetGraphicsRootConstantBufferView(
			_ConstantBufferRVStatic<T>::m_RootIndex,
			_ConstantBufferRVStatic<T>::m_pBuffer->gpuPTR
		);
	}
};

template<typename T>
class ConstantBufferCBVDynamic : public _ConstantBufferRVDynamic<T> {
public:
	using _ConstantBufferRVDynamic<T>::_ConstantBufferRVDynamic;

	void BindCommand(Graphics& gfx) noexcept override {
		T constData = _ConstantBufferRVDynamic<T>::m_Getter();

		memcpy(
			_ConstantBufferRVDynamic<T>::m_pBuffer->cpuPTR,
			&constData,
			static_cast<std::uint32_t>(sizeof(T))
		);

		Bindable::GetCommandList(gfx)->SetGraphicsRootConstantBufferView(
			_ConstantBufferRVDynamic<T>::m_RootIndex,
			_ConstantBufferRVDynamic<T>::m_pBuffer->gpuPTR
		);
	}
};

typedef ConstantBuffer<DirectX::XMMATRIX> ConstantBufferMat;
#endif
