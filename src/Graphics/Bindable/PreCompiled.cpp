#include <PreCompiled.hpp>
#include <d3dcompiler.h>
#include <GraphicsThrowMacros.hpp>

PreCompiled::PreCompiled(const std::wstring& fileName) {
	HRESULT hr;
	GFX_THROW_FAILED(hr, D3DReadFileToBlob(fileName.c_str(), &m_pBinary));
}
