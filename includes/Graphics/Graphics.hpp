#ifndef __GRAPHICS_HPP__
#define __GRAPHICS_HPP__
#include <CleanWin.hpp>
#include <DxgiInfoManager.hpp>
#include <d3d11.h>
#include <wrl.h>

using Microsoft::WRL::ComPtr;

class Graphics {

public:
	Graphics(HWND hwnd, std::uint32_t width, std::uint32_t height);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics() = default;

	void EndFrame();
	void ClearBuffer(float red, float green, float blue) noexcept;
	void DrawTriangle(float angle, float posX, float posY);

private:
	void SetShaderPath() noexcept;

private:
	ComPtr<ID3D11Device> m_pDevice;
	ComPtr<IDXGISwapChain> m_pSwapChain;
	ComPtr<ID3D11DeviceContext> m_pDeviceContext;
	ComPtr<ID3D11RenderTargetView> m_pTargetView;
	ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;

	std::wstring m_ShaderPath;
	std::uint32_t m_width;
	std::uint32_t m_height;
};
#endif