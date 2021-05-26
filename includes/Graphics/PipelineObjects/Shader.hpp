#ifndef __SHADER_HPP__
#define __SHADER_HPP__
#include <PreCompiled.hpp>

class Shader : public PreCompiled {
public:
	Shader() = default;

	Shader(const std::wstring& fileName);

	void Init(const std::wstring& fileName);

	D3D12_SHADER_BYTECODE GetShaderByteCode() const noexcept;

protected:
	D3D12_SHADER_BYTECODE m_ByteCode;
};
#endif
