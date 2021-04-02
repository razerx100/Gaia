#ifndef __PRE_COMPILED_HPP__
#define __PRE_COMPILED_HPP__
#include <string>
#include <CleanWin.hpp>
#include <wrl.h>
#include <d3d12.h>

using Microsoft::WRL::ComPtr;

class PreCompiled {
public:
	PreCompiled(const std::wstring& fileName);

protected:
	ComPtr<ID3DBlob> m_pBinary;
};
#endif
