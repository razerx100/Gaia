#ifndef __ROOT_SIGNATURE_HPP__
#define __ROOT_SIGNATURE_HPP__
#include <Bindable.hpp>
#include <PreCompiled.hpp>

class RootSignature : public Bindable, public PreCompiled {
public:
	RootSignature(Graphics& gfx, const std::wstring& fileName);

	void BindCommand(Graphics& gfx) noexcept override;
	ID3D12RootSignature* GetSignature() const noexcept;

private:
	ComPtr<ID3D12RootSignature> m_pRootSignature;
};
#endif
