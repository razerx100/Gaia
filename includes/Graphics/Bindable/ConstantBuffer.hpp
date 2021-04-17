#ifndef __CONSTANT_BUFFER_HPP__
#define __CONSTANT_BUFFER_HPP__
#include <Bindable.hpp>
#include <Drawable.hpp>
#include <memory>
#include <functional>
#include <GraphicsThrowMacros.hpp>
#include <d3dx12.h>

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
		: m_RSIndex(indexInRootSignature), m_ConstantInfo{elementsNumber, getter} {

		m_bindFunction = std::bind(&ConstantBuffer<T>::BindConstant, this, std::placeholders::_1);
	}

	// 256bits aligned bufferSize
	ConstantBuffer(std::uint32_t indexInRootSignature,
		std::uint32_t bufferSize, std::uint8_t **cpuPtr, Graphics& gfx)
		:m_RSIndex(indexInRootSignature) {

		{
			D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc = {};
			cbvHeapDesc.NumDescriptors = 1;
			cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

			GFX_THROW_FAILED(hr, GetDevice(gfx)->CreateDescriptorHeap(
				&cbvHeapDesc, __uuidof(ID3D12DescriptorHeap), &m_CopyHeap
			));

			CD3DX12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

			CD3DX12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

			GFX_THROW_FAILED(hr, GetDevice(gfx)->CreateCommittedResource(
				&heapProp,
				D3D12_HEAP_FLAG_NONE,
				&resDesc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				__uuidof(ID3D12Resource),
				&m_HeapResource
			));

			D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
			cbvDesc.BufferLocation = m_HeapResource->GetGPUVirtualAddress();
			cbvDesc.SizeInBytes = bufferSize;

			GetDevice(gfx)->CreateConstantBufferView(
				&cbvDesc, m_CopyHeap->GetCPUDescriptorHandleForHeapStart()
			);

			GFX_THROW_FAILED(
				hr, m_HeapResource->Map(0, nullptr, reinterpret_cast<void**>(cpuPtr))
			);
		}
		m_HeapIndex = GetSRVHeapMan(gfx).RequestHandleIndex(
			m_CopyHeap->GetCPUDescriptorHandleForHeapStart()
		);

		m_bindFunction = std::bind(&ConstantBuffer<T>::BindDescriptor, this, std::placeholders::_1);
	}

	void BindConstant(Graphics& gfx) {
		T constData = m_ConstantInfo.getter();

		GetCommandList(gfx)->SetGraphicsRoot32BitConstants(
			m_RSIndex,
			m_ConstantInfo.elements,
			&constData, 0
		);
	}

	void BindDescriptor(Graphics& gfx) {
		GetCommandList(gfx)->SetGraphicsRootDescriptorTable(
			m_RSIndex, GetSRVHeapMan(gfx).RequestHandleGPU(m_HeapIndex)
		);
	}

	void BindCommand(Graphics& gfx) noexcept override {
		m_bindFunction(gfx);
	}

protected:
	std::uint32_t m_RSIndex;
	std::uint32_t m_HeapIndex;

	std::function<void(Graphics&)> m_bindFunction;

	ComPtr<ID3D12Resource> m_HeapResource;
	ComPtr<ID3D12DescriptorHeap> m_CopyHeap;
};

class VertexConstantBuffer : public ConstantBuffer<DirectX::XMMATRIX> {
public:
	using ConstantBuffer<DirectX::XMMATRIX>::ConstantBuffer;

	void BindCommand(Graphics& gfx) noexcept override;
};

#endif
