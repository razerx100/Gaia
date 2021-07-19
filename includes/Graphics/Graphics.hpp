#ifndef __GRAPHICS_HPP__
#define __GRAPHICS_HPP__
#include <CleanWin.hpp>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <d3dx12.h>
#include <HeapMan.hpp>
#include <CommandQueue.hpp>

using Microsoft::WRL::ComPtr;

class Graphics {
    friend class GraphicsExtractor;
public:
	Graphics(HWND hwnd, std::uint32_t width, std::uint32_t height);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics();

	void EndFrame();
    void BeginFrame();
    void PresentFrame();
    void DrawIndexed(
        ID3D12GraphicsCommandList* commandList,
        std::uint32_t indexCount
    ) noexcept;
    void RecordCommonCommands(ID3D12GraphicsCommandList* commandList);
    void InitialGPUSetup();
    void ResizeBuffer(std::uint32_t width, std::uint32_t height);
    void SetBGColor(float red, float green, float blue);

    DXGI_OUTPUT_DESC GetOutputDesc();

    static void SetRenderFormat(DXGI_FORMAT renderFormat) noexcept;
    static DXGI_FORMAT GetRenderFormat() noexcept;

private:
    void Initialize(HWND hwnd);
	void WaitForGPU();
	void MoveToNextFrame();
    void CreateRTVs();
    void CreateDSV();
    void InitializeViewPortAndSRECT();
    void GetHardwareAdapter(
        IDXGIFactory1* pFactory,
        IDXGIAdapter1** ppAdapter
    );
    void TransitionIntoPresentMode(
        ID3D12Resource* rtv,
        ID3D12GraphicsCommandList* commandList
    );
    void TransitionIntoRenderMode(
        ID3D12Resource* rtv,
        ID3D12GraphicsCommandList* commandList
    );

private:
	static constexpr std::uint16_t bufferCount = 2u;
    static  DXGI_FORMAT s_renderFormat;

    D3D12_VIEWPORT m_viewport;
    D3D12_RECT m_scissorRect;
    ComPtr<IDXGISwapChain4> m_pSwapChain;
    ComPtr<ID3D12Device5> m_pDevice;
    ComPtr<ID3D12Resource> m_pRenderTargets[bufferCount];
    CommandQueue m_gfxCommandQueue;
    CommandList* m_gfxCommandListRef;

    ComPtr<ID3D12DescriptorHeap> m_pRTVHeap;
    ComPtr<ID3D12DescriptorHeap> m_pDSVHeap;

    std::unique_ptr<HeapMan> m_pSRVHeapMan;

    std::uint32_t m_nRTVHeapIncSize;

    ComPtr<ID3D12Resource> m_pDepthBuffer;

    ComPtr<ID3D12Fence> m_pFence;
    HANDLE m_fenceEvent;
    std::uint64_t m_fenceValues[bufferCount];
    std::uint32_t m_currentBackBufferIndex;

	std::uint32_t m_width;
	std::uint32_t m_height;

	float m_color[4];
	HRESULT hr;
};
#endif