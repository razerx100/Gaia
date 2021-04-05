#ifndef __SAMPLER_HPP__
#define __SAMPLER_HPP__
#include <Bindable.hpp>

class Sampler : public Bindable {
public:
	Sampler(Graphics& gfx);

	void Bind(Graphics& gfx) noexcept override;

private:
	ComPtr<ID3D11SamplerState> m_pSampler;
};
#endif
