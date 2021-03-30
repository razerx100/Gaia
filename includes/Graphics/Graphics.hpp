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
	~Graphics();

	void EndFrame();
	void ClearBuffer(float red, float green, float blue);
	//void DrawTriangle(float angle, float posX, float posY);

private:
	void WaitForGPU();
	void MoveToNextFrame();

	void SetShaderPath() noexcept;

private:
	static constexpr std::uint16_t bufferCount = 2u;

	ComPtr<ID3D12Device2> m_pDevice;
	ComPtr<IDXGISwapChain4> m_pSwapChain;
	ComPtr<ID3D12CommandQueue> m_pCommandQueue;
	ComPtr<ID3D12Resource> m_pRenderTargets[bufferCount];
	ComPtr<ID3D12CommandAllocator> m_pCommandAllocator[bufferCount];
	ComPtr<ID3D12GraphicsCommandList> m_pCommandList;
	ComPtr<ID3D12DescriptorHeap> m_pRTVHeap;
	ComPtr<ID3D12DescriptorHeap> m_pSRVHeap;

	std::uint32_t m_RTVSize;
	std::uint32_t m_CurrentBackBufferIndex;

	ComPtr<ID3D12Fence> m_Fence;
	std::uint64_t m_FenceValues[bufferCount];
	HANDLE m_FenceEvent;

	D3D12_VIEWPORT m_ViewPort;
	D3D12_RECT m_ScissorRect;
	std::wstring m_ShaderPath;
	std::uint32_t m_width;
	std::uint32_t m_height;

	HRESULT hr;
};
#endif