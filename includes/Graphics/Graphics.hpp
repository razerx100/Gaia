#ifndef __GRAPHICS_HPP__
#define __GRAPHICS_HPP__
#include <CleanWin.hpp>
#include <DxgiInfoManager.hpp>
#include <d3d12.h>
#include <dxgi1_6.h>
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

	ComPtr<ID3D12Device> m_pDevice;
	ComPtr<IDXGISwapChain3> m_pSwapChain;
	ComPtr<ID3D12CommandQueue> m_pCommandQueue;
	ComPtr<ID3D12CommandAllocator> m_pCommandAllocator;
	ComPtr<ID3D12GraphicsCommandList> m_pCommandList;
	//ComPtr<ID3D11RenderTargetView> m_pTargetView;
	//ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;

	std::wstring m_ShaderPath;
	std::uint32_t m_width;
	std::uint32_t m_height;
};
#endif