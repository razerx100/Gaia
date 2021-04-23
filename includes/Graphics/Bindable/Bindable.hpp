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
#endif
