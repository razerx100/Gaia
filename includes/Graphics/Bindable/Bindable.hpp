#ifndef __BINDABLE_HPP__
#define __BINDABLE_HPP__
#include <Graphics.hpp>

class Bindable {
public:
	Bindable() = default;
	virtual ~Bindable() = default;

	virtual void BindCommand(Graphics& gfx) noexcept = 0;

protected:
	static ID3D12Device2* GetDevice(Graphics& gfx) noexcept;
	static ID3D12GraphicsCommandList* GetCommandList(Graphics& gfx) noexcept;

protected:
	HRESULT hr;
};

class BindableResource : public Bindable {
protected:
	void CreateResource(
		Graphics& gfx, void* data, std::uint32_t dataSize, void** buffer,
		D3D12_HEAP_TYPE heapType = D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATES resourceStates = D3D12_RESOURCE_STATE_GENERIC_READ
	);
};
#endif
