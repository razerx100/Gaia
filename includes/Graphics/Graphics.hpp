#ifndef __GRAPHICS_HPP__
#define __GRAPHICS_HPP__
#include <CleanWin.hpp>
#include <DebugInfoManager.hpp>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <d3dx12.h>

using Microsoft::WRL::ComPtr;

class Graphics {

public:
	Graphics(HWND hwnd, std::uint32_t width, std::uint32_t height);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics();

	void EndFrame();
    void ResetCommandList();
    void ClearBuffer(float red, float green, float blue);
    void DrawTriangle();

private:

    void LoadPipeline(HWND hwnd);

    void LoadTriangle();


	void WaitForGPU();
	void MoveToNextFrame();

	void SetShaderPath() noexcept;

private:
	static constexpr std::uint16_t bufferCount = 2u;

    CD3DX12_VIEWPORT m_Viewport;
    CD3DX12_RECT m_ScissorRect;
    ComPtr<IDXGISwapChain4> m_pSwapChain;
    ComPtr<ID3D12Device2> m_pDevice;
    ComPtr<ID3D12Resource> m_pRenderTargets[bufferCount];
    ComPtr<ID3D12CommandAllocator> m_pCommandAllocators[bufferCount];
    ComPtr<ID3D12CommandQueue> m_pCommandQueue;
    ComPtr<ID3D12RootSignature> m_pRootSignature;
    ComPtr<ID3D12DescriptorHeap> m_pRTVHeap;
    ComPtr<ID3D12DescriptorHeap> m_pDSVHeap;
    ComPtr<ID3D12PipelineState> m_pPipelineState;
    ComPtr<ID3D12GraphicsCommandList> m_pCommandList;
    std::uint32_t m_RTVHeapSize;

    ComPtr<ID3D12Resource> m_pVertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView;

    ComPtr<ID3D12Resource> m_pIndexBuffer;
    D3D12_INDEX_BUFFER_VIEW m_IndexBufferView;

    std::uint32_t m_CurrentBackBufferIndex;
    HANDLE m_FenceEvent;
    ComPtr<ID3D12Fence> m_pFence;
    std::uint64_t m_FenceValues[bufferCount];

	std::wstring m_ShaderPath;
	std::uint32_t m_width;
	std::uint32_t m_height;

    std::uint32_t m_triangleIndicesCount;

	float m_color[4];
	HRESULT hr;
};
#endif