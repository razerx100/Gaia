#ifndef __SHADER_HPP__
#define __SHADER_HPP__
#include <PreCompiled.hpp>

class Shader : public PreCompiled {
public:
	Shader() = default;
	Shader(const std::string& fileName);

	void Init(const std::string& fileName);

	D3D12_SHADER_BYTECODE GetShaderByteCode() const noexcept;

private:
	D3D12_SHADER_BYTECODE m_ByteCode;
};
#endif
