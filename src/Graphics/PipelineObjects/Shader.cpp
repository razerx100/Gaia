#include <Shader.hpp>

Shader::Shader(const std::string& fileName)
	: PreCompiled(fileName), m_ByteCode{} {
	m_ByteCode.BytecodeLength = m_pBinary->GetBufferSize();
	m_ByteCode.pShaderBytecode = m_pBinary->GetBufferPointer();
}

void Shader::Init(const std::string& fileName) {
	ReadBinary(fileName);

	m_ByteCode.BytecodeLength = m_pBinary->GetBufferSize();
	m_ByteCode.pShaderBytecode = m_pBinary->GetBufferPointer();
}

D3D12_SHADER_BYTECODE Shader::GetShaderByteCode() const noexcept {
	return m_ByteCode;
}
