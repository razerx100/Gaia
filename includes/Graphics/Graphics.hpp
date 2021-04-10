#ifndef __GRAPHICS_HPP__
#define __GRAPHICS_HPP__
#include <CleanWin.hpp>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <d3dx12.h>
#include <HeapMan.hpp>

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
    void DrawIndexed(std::uint32_t indexCount) noexcept;
	void WaitForGPU();
    void InitialGPUSetup();

    void EnableImGui() noexcept;
    void DisableImGui() noexcept;
    bool IsImGuiEnabled() const noexcept;

private:
    void Initialize(HWND hwnd);
    void ExecuteCommandList();
    void ResetCommandList();

    void ImGuiBegin();
    void ImGuiEnd();

	void MoveToNextFrame();

private:
	static constexpr std::uint16_t bufferCount = 2u;

    CD3DX12_VIEWPORT m_Viewport;
    CD3DX12_RECT m_ScissorRect;
    ComPtr<IDXGISwapChain4> m_pSwapChain;
    ComPtr<ID3D12Device2> m_pDevice;
    ComPtr<ID3D12Resource> m_pRenderTargets[bufferCount];
    ComPtr<ID3D12CommandAllocator> m_pCommandAllocators[bufferCount];
    ComPtr<ID3D12CommandQueue> m_pCommandQueue;
    ComPtr<ID3D12GraphicsCommandList> m_pCommandList;

    ComPtr<ID3D12DescriptorHeap> m_pRTVHeap;
    ComPtr<ID3D12DescriptorHeap> m_pDSVHeap;

    std::unique_ptr<HeapMan> m_SRVHeapMan;

    std::uint32_t m_RTVHeapIncSize;

    ComPtr<ID3D12Resource> m_pDepthBuffer;

    ComPtr<ID3D12Fence> m_pFence;
    HANDLE m_FenceEvent;
    std::uint32_t m_CurrentBackBufferIndex;
    std::uint64_t m_FenceValues[bufferCount];

	std::wstring m_ShaderPath;
	std::uint32_t m_width;
	std::uint32_t m_height;

    std::uint32_t m_triangleIndicesCount;

	float m_color[4];
    bool m_imGuiEnabled;
	HRESULT hr;
};
#endif