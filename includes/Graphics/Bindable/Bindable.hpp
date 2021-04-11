#ifndef __BINDABLE_HPP__
#define __BINDABLE_HPP__
#include <Graphics.hpp>

class Bindable {
public:
	virtual void Bind(Graphics& gfx) noexcept = 0;
	virtual ~Bindable() = default;

protected:
	static ID3D11DeviceContext* GetDeviceContext(Graphics& gfx) noexcept;
	static ID3D11Device* GetDevice(Graphics& gfx) noexcept;

protected:
	 HRESULT hr;
};
#endif
