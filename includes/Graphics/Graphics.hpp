#ifndef __GRAPHICS_HPP__
#define __GRAPHICS_HPP__
#include <CleanWin.hpp>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <d3dx12.h>
#include <HeapMan.hpp>
#include <CommandList.hpp>

using Microsoft::WRL::ComPtr;

class Graphics {
    friend class GraphicsExtractor;
public:
	Graphics(HWND hwnd, std::uint32_t width, std::uint32_t height);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics();

	void EndFrame();
    void BeginFrame(float red, float green, float blue);
    void PresentFrame();
    void DrawIndexed(std::uint32_t indexCount) noexcept;
    void InitialGPUSetup();
    void ResizeBuffer(std::uint32_t width, std::uint32_t height);

    DXGI_OUTPUT_DESC GetOutputDesc();

private:
    void Initialize(HWND hwnd);
    void ExecuteCommandList();
    void ResetCommandList();
	void WaitForGPU();
	void MoveToNextFrame();
    void CreateRTVs();
    void CreateDSV();
    void InitializeViewPortAndSRECT();

private:
	static constexpr std::uint16_t bufferCount = 2u;

    D3D12_VIEWPORT m_viewport;
    D3D12_RECT m_scissorRect;
    ComPtr<IDXGISwapChain4> m_pSwapChain;
    ComPtr<ID3D12Device2> m_pDevice;
    ComPtr<ID3D12Resource> m_pRenderTargets[bufferCount];
    ComPtr<ID3D12CommandQueue> m_pCommandQueue;
    CommandList m_gfxCommandList;

    ComPtr<ID3D12DescriptorHeap> m_pRTVHeap;
    ComPtr<ID3D12DescriptorHeap> m_pDSVHeap;

    std::unique_ptr<HeapMan> m_pSRVHeapMan;

    std::uint32_t m_nRTVHeapIncSize;

    ComPtr<ID3D12Resource> m_pDepthBuffer;

    ComPtr<ID3D12Fence> m_pFence;
    HANDLE m_fenceEvent;
    std::uint32_t m_currentBackBufferIndex;
    std::uint64_t m_fenceValues[bufferCount];

	std::uint32_t m_width;
	std::uint32_t m_height;

	float m_color[4];
	HRESULT hr;
};
#endif