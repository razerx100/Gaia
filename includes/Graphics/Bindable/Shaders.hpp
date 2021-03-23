#ifndef __SHADERS_HPP__
#define __SHADERS_HPP__
#include <Bindable.hpp>
#include <string>
#include <d3dcompiler.h>

class Shader : public Bindable {
public:
	ID3DBlob* GetByteCode() const noexcept;

protected:
	ComPtr<ID3DBlob> m_pByteCode;
};

class PixelShader : public Shader {
public:
	PixelShader(Graphics& gfx, const std::wstring& path);

	void Bind(Graphics& gfx) noexcept;

protected:
	ComPtr<ID3D11PixelShader> m_pPixelShader;
};

class VertexShader : public Shader {
public:
	VertexShader(Graphics& gfx, const std::wstring& path);

	void Bind(Graphics& gfx) noexcept;

protected:
	ComPtr<ID3D11VertexShader> m_pVertexShader;
};
#endif
