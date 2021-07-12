#include <PreCompiled.hpp>
#include <d3dcompiler.h>
#include <GraphicsThrowMacros.hpp>

PreCompiled::PreCompiled(const std::string& fileName) {
	HRESULT hr;
	GFX_THROW_FAILED(hr, D3DReadFileToBlob(StrToWStr(fileName).c_str(), &m_pBinary));
}

void PreCompiled::ReadBinary(const std::string& fileName) {
	HRESULT hr;
	GFX_THROW_FAILED(hr, D3DReadFileToBlob(StrToWStr(fileName).c_str(), &m_pBinary));
}

std::wstring PreCompiled::StrToWStr(const std::string& str) const noexcept {
	return std::wstring(str.begin(), str.end());
}
