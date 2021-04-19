#include <Texture.hpp>
#include <Surface.hpp>
#include <GraphicsThrowMacros.hpp>
#include <d3dx12.h>
#include <HeapMan.hpp>

Texture::Texture(Graphics& gfx, const Surface& s) {

	{
		D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
		srvHeapDesc.NumDescriptors = 1;
		srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

		GFX_THROW_FAILED(hr, GetDevice(gfx)->CreateDescriptorHeap(
			&srvHeapDesc, __uuidof(ID3D12DescriptorHeap), &m_pSRVHeap
		));
	}

	D3D12_RESOURCE_DESC texDesc = {};
	texDesc.Height = s.GetHeight();
	texDesc.Width = s.GetWidth();
	texDesc.MipLevels = 1;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.SampleDesc.Count = 1;
	texDesc.DepthOrArraySize = 1;
	texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	CD3DX12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	GFX_THROW_FAILED(hr, GetDevice(gfx)->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&texDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		__uuidof(ID3D12Resource),
		&m_pTexture
	));

	const UINT64 uploadBufferSize = GetRequiredIntermediateSize(m_pTexture.Get(), 0, 1);

	heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

	CD3DX12_RESOURCE_DESC rsDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);

	GFX_THROW_FAILED(hr, GetDevice(gfx)->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&rsDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		__uuidof(ID3D12Resource),
		&m_pUploadHeap
	));


	D3D12_SUBRESOURCE_DATA sRC = {};
	sRC.pData = s.GetBufferPtr();
	sRC.RowPitch = s.GetWidth() * sizeof(Surface::Color);
	sRC.SlicePitch = sRC.RowPitch * s.GetHeight();

	UpdateSubresources(
		GetCommandList(gfx), m_pTexture.Get(), m_pUploadHeap.Get(), 0, 0, 1, &sRC
	);

	D3D12_RESOURCE_BARRIER transition = CD3DX12_RESOURCE_BARRIER::Transition(
		m_pTexture.Get(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
	);

	GetCommandList(gfx)->ResourceBarrier(1, &transition);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	GFX_THROW_NO_HR(GetDevice(gfx)->CreateShaderResourceView(
		m_pTexture.Get(), &srvDesc, m_pSRVHeap->GetCPUDescriptorHandleForHeapStart()
	))

		m_SRVIndex = GetSRVHeapMan(gfx).RequestHandleIndex(
			m_pSRVHeap->GetCPUDescriptorHandleForHeapStart(),
			[&](D3D12_GPU_DESCRIPTOR_HANDLE handle) {
				m_GPUHandle = handle;
			}
	);
}

void Texture::OnDestroy(Graphics& gfx) noexcept {
	GetSRVHeapMan(gfx).Free(m_SRVIndex);
}

void Texture::BindCommand(Graphics& gfx) noexcept {

	GetCommandList(gfx)->SetGraphicsRootDescriptorTable(
		1, m_GPUHandle
	);
}

