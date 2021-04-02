#include <Bindable.hpp>
#include <d3dx12.h>
#include <GraphicsThrowMacros.hpp>

ID3D12Device2* Bindable::GetDevice(Graphics& gfx) noexcept {
	return gfx.m_pDevice.Get();
}
ID3D12GraphicsCommandList* Bindable::GetCommandList(Graphics& gfx) noexcept {
	return gfx.m_pCommandList.Get();
}

void BindableResource::CreateResource(
    Graphics& gfx, void* data, std::uint32_t dataSize, void** buffer,
    D3D12_HEAP_TYPE heapType,
    D3D12_RESOURCE_STATES resourceStates
) {
    CD3DX12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(heapType);

    CD3DX12_RESOURCE_DESC rcDesc = CD3DX12_RESOURCE_DESC::Buffer(dataSize);

    GFX_THROW_FAILED(hr, GetDevice(gfx)->CreateCommittedResource(
        &heapProp,
        D3D12_HEAP_FLAG_NONE,
        &rcDesc,
        resourceStates,
        nullptr,
        __uuidof(ID3D12Resource),
        buffer
    ));

    UINT8* pDataBegin;
    CD3DX12_RANGE readRange(0, 0);

    ID3D12Resource* pBuffer = *(reinterpret_cast<ID3D12Resource**>(buffer));

    GFX_THROW_FAILED(hr, pBuffer->Map(
        0, &readRange, reinterpret_cast<void**>(&pDataBegin)));
    memcpy(pDataBegin, data, dataSize);
    pBuffer->Unmap(0, nullptr);
}
