#include <Bindable.hpp>

ID3D11DeviceContext* Bindable::GetDeviceContext(Graphics& gfx) noexcept {
	return gfx.m_pDeviceContext.Get();
}

ID3D11Device* Bindable::GetDevice(Graphics& gfx) noexcept {
	return gfx.m_pDevice.Get();
}
