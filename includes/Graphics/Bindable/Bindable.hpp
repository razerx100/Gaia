#ifndef __BINDABLE_HPP__
#define __BINDABLE_HPP__
#include <CleanWin.hpp>
#include <GraphicsExtractor.hpp>
#include <cstdint>
#include <wrl.h>

using Microsoft::WRL::ComPtr;

class Bindable : public GraphicsExtractor {
public:
	Bindable() = default;
	virtual ~Bindable() = default;

	virtual void BindCommand(Graphics& gfx) noexcept = 0;

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
