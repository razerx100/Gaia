#ifndef __TEXTURE_HPP__
#define __TEXTURE_HPP__
#include <Bindable.hpp>

class Texture : public Bindable {
public:
	Texture(Graphics& gfx, const class Surface& s);

	void Bind(Graphics& gfx) noexcept override;

private:
	ComPtr<ID3D11ShaderResourceView> m_pTextureView;
};

#endif
