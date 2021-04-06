#include <RootSignature.hpp>
#include <GraphicsThrowMacros.hpp>
#include <Graphics.hpp>

RootSignature::RootSignature(Graphics& gfx, const std::wstring& fileName)
	: PreCompiled(fileName) {
	GFX_THROW_FAILED(hr,
			GetDevice(gfx)->CreateRootSignature(
				0u,
				m_pBinary->GetBufferPointer(),
				m_pBinary->GetBufferSize(),
				__uuidof(ID3D12RootSignature),
				&m_pRootSignature
			)
		);
}

void RootSignature::BindCommand(Graphics& gfx) noexcept {
	GetCommandList(gfx)->SetGraphicsRootSignature(m_pRootSignature.Get());
}

ID3D12RootSignature* RootSignature::GetSignature() const noexcept {
	return m_pRootSignature.Get();
}
