#include <Graphics.hpp>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <GraphicsThrowMacros.hpp>
#include <imgui_impl_dx12.h>
#include <imgui_impl_win32.h>

// Graphics
Graphics::Graphics(HWND hwnd, std::uint32_t width, std::uint32_t height)
	: m_pDevice(nullptr),
	m_pSwapChain(nullptr),
	m_width(width), m_height(height), m_color{0.0f, 0.0f, 0.0f, 1.0f},
	m_FenceValues{}, m_RTVHeapIncSize(0), m_CurrentBackBufferIndex(0),
    m_Viewport{ 0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height) },
    m_ScissorRect{0, 0, static_cast<long>(width), static_cast<long>(height)},
    m_triangleIndicesCount(0u), m_imGuiEnabled(true), hr(0) {

    Initialize(hwnd);
    ImGui_ImplDX12_Init(
        m_pDevice.Get(), bufferCount, DXGI_FORMAT_B8G8R8A8_UNORM, m_SRVHeapMan.get()
    );
}

Graphics::~Graphics() {
	WaitForGPU();

	CloseHandle(m_FenceEvent);

    ImGui_ImplDX12_Shutdown();
}

void Graphics::Initialize(HWND hwnd) {
    std::uint32_t dxgiFactoryFlags = 0;

#ifdef _DEBUG
    {
        ComPtr<ID3D12Debug> debugController;
        D3D12GetDebugInterface(__uuidof(ID3D12Debug), &debugController);
        debugController->EnableDebugLayer();

        dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
    }
#endif

    ComPtr<IDXGIFactory4> factory;
    CreateDXGIFactory2(dxgiFactoryFlags, __uuidof(IDXGIFactory4), &factory);

    {
        ComPtr<IDXGIAdapter1> adapter;

        factory->EnumAdapters1(
            0u, &adapter);

        D3D12CreateDevice(
            adapter.Get(),
            D3D_FEATURE_LEVEL_11_0,
            __uuidof(ID3D12Device2),
            &m_pDevice
        );
    }

#ifdef _DEBUG
    DebugInfoManager::SetDebugInfoManager(m_pDevice.Get());
#endif

    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    GFX_THROW_FAILED(hr, m_pDevice->CreateCommandQueue(
        &queueDesc, __uuidof(ID3D12CommandQueue), &m_pCommandQueue
    ));

    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.BufferCount = bufferCount;
    swapChainDesc.Width = m_width;
    swapChainDesc.Height = m_height;
    swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

    ComPtr<IDXGISwapChain1> swapChain;
    GFX_THROW_FAILED(hr, factory->CreateSwapChainForHwnd(
        m_pCommandQueue.Get(),
        hwnd,
        &swapChainDesc,
        nullptr,
        nullptr,
        &swapChain
    ));

    GFX_THROW_FAILED(hr, swapChain.As(&m_pSwapChain));
    m_CurrentBackBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();

    // RTV
    {
        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
        rtvHeapDesc.NumDescriptors = bufferCount;
        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

        GFX_THROW_FAILED(hr, m_pDevice->CreateDescriptorHeap(
            &rtvHeapDesc, __uuidof(ID3D12DescriptorHeap), &m_pRTVHeap
        ));

        m_RTVHeapIncSize = m_pDevice->GetDescriptorHandleIncrementSize(
            D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    }

    {
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_pRTVHeap->GetCPUDescriptorHandleForHeapStart());

        for (std::uint32_t n = 0; n < bufferCount; n++) {
            GFX_THROW_FAILED(hr, m_pSwapChain->GetBuffer(
                n, __uuidof(ID3D12Resource), &m_pRenderTargets[n])
            );

            GFX_THROW_NO_HR(
                m_pDevice->CreateRenderTargetView(m_pRenderTargets[n].Get(), nullptr, rtvHandle)
            )
            rtvHandle.Offset(1, m_RTVHeapIncSize);

            GFX_THROW_FAILED(hr, m_pDevice->CreateCommandAllocator(
                D3D12_COMMAND_LIST_TYPE_DIRECT,
                __uuidof(ID3D12CommandAllocator),
                &m_pCommandAllocators[n]
            ));
        }
    }

    // DSV
    {
        D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
        dsvHeapDesc.NumDescriptors = 1;
        dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

        GFX_THROW_FAILED(hr, m_pDevice->CreateDescriptorHeap(
            &dsvHeapDesc, __uuidof(ID3D12DescriptorHeap), &m_pDSVHeap
        ));

        D3D12_CLEAR_VALUE depthValue = {};
        depthValue.Format = DXGI_FORMAT_D32_FLOAT;
        depthValue.DepthStencil = { 1.0f, 0 };

        CD3DX12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

        CD3DX12_RESOURCE_DESC rsDesc = CD3DX12_RESOURCE_DESC::Tex2D(
            DXGI_FORMAT_D32_FLOAT, m_width, m_height,
            1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
        );

        GFX_THROW_FAILED(hr, m_pDevice->CreateCommittedResource(
            &heapProp,
            D3D12_HEAP_FLAG_NONE,
            &rsDesc,
            D3D12_RESOURCE_STATE_DEPTH_WRITE,
            &depthValue,
            __uuidof(ID3D12Resource),
            &m_pDepthBuffer
        ));

        D3D12_DEPTH_STENCIL_VIEW_DESC dsv = {};
        dsv.Format = DXGI_FORMAT_D32_FLOAT;
        dsv.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
        dsv.Texture2D.MipSlice = 0;
        dsv.Flags = D3D12_DSV_FLAG_NONE;

        GFX_THROW_NO_HR(
            m_pDevice->CreateDepthStencilView(
                m_pDepthBuffer.Get(), &dsv,
                m_pDSVHeap->GetCPUDescriptorHandleForHeapStart()
            )
        )
    }

    GFX_THROW_FAILED(hr, m_pDevice->CreateFence(
        m_FenceValues[m_CurrentBackBufferIndex],
        D3D12_FENCE_FLAG_NONE,
        __uuidof(ID3D12Fence),
        &m_pFence
    ));
    m_FenceValues[m_CurrentBackBufferIndex]++;

    m_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (m_FenceEvent == nullptr)
        GFX_THROW_FAILED(hr, HRESULT_FROM_WIN32(GetLastError()));

    // Command List
    GFX_THROW_FAILED(hr, m_pDevice->CreateCommandList(
        0, D3D12_COMMAND_LIST_TYPE_DIRECT,
        m_pCommandAllocators[m_CurrentBackBufferIndex].Get(),
        nullptr,
        __uuidof(ID3D12CommandList),
        &m_pCommandList
    ));

    m_SRVHeapMan = std::make_unique<HeapMan>(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, *this);
}

void Graphics::BeginFrame(float red, float green, float blue) {
    ResetCommandList();

    ImGuiBegin();

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(
        m_pRTVHeap->GetCPUDescriptorHandleForHeapStart(),
        m_CurrentBackBufferIndex, m_RTVHeapIncSize);

    m_color[0] = red;
    m_color[1] = green;
    m_color[2] = blue;

    m_pCommandList->ClearRenderTargetView(rtvHandle, m_color, 0, nullptr);

    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = m_pDSVHeap->GetCPUDescriptorHandleForHeapStart();

    m_pCommandList->ClearDepthStencilView(
        dsvHandle,
        D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

    m_pCommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);
}

void Graphics::DrawIndexed(std::uint32_t indexCount) noexcept {
    m_pCommandList->DrawIndexedInstanced(indexCount, 1u, 0u, 0u, 0u);
}

void Graphics::ResetCommandList() {
    GFX_THROW_FAILED(hr, m_pCommandAllocators[m_CurrentBackBufferIndex]->Reset());

    GFX_THROW_FAILED(hr, m_pCommandList->Reset(
        m_pCommandAllocators[m_CurrentBackBufferIndex].Get(), nullptr));

    ID3D12DescriptorHeap* ppHeaps[] = {
        m_SRVHeapMan->GetHeap()
    };

    m_pCommandList->SetDescriptorHeaps(
        1, ppHeaps
    );

    m_pCommandList->RSSetViewports(1, &m_Viewport);
    m_pCommandList->RSSetScissorRects(1, &m_ScissorRect);

    CD3DX12_RESOURCE_BARRIER prebar = CD3DX12_RESOURCE_BARRIER::Transition(
        m_pRenderTargets[m_CurrentBackBufferIndex].Get(),
        D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

    m_pCommandList->ResourceBarrier(1, &prebar);
}

void Graphics::WaitForGPU() {
    GFX_THROW_FAILED(hr, m_pCommandQueue->Signal(
        m_pFence.Get(), m_FenceValues[m_CurrentBackBufferIndex]));

    GFX_THROW_FAILED(hr,
        m_pFence->SetEventOnCompletion(
            m_FenceValues[m_CurrentBackBufferIndex], m_FenceEvent));
    WaitForSingleObjectEx(m_FenceEvent, INFINITE, FALSE);

    m_FenceValues[m_CurrentBackBufferIndex]++;
}

void Graphics::MoveToNextFrame() {
    const std::uint64_t currentFenceValue = m_FenceValues[m_CurrentBackBufferIndex];
    GFX_THROW_FAILED(hr, m_pCommandQueue->Signal(m_pFence.Get(), currentFenceValue));

    m_CurrentBackBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();

    if (m_pFence->GetCompletedValue() < m_FenceValues[m_CurrentBackBufferIndex]) {
        GFX_THROW_FAILED(hr,
            m_pFence->SetEventOnCompletion(
                m_FenceValues[m_CurrentBackBufferIndex], m_FenceEvent));
        WaitForSingleObjectEx(m_FenceEvent, INFINITE, FALSE);
    }

    m_FenceValues[m_CurrentBackBufferIndex] = currentFenceValue + 1;
}

void Graphics::EndFrame() {
    if (m_imGuiEnabled)
        ImGuiEnd();

    CD3DX12_RESOURCE_BARRIER postbar = CD3DX12_RESOURCE_BARRIER::Transition(
        m_pRenderTargets[m_CurrentBackBufferIndex].Get(),
        D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

    m_pCommandList->ResourceBarrier(1, &postbar);

    ExecuteCommandList();

    GFX_THROW_FAILED(hr, m_pSwapChain->Present(0, DXGI_PRESENT_ALLOW_TEARING));

    m_SRVHeapMan->ProcessRequests();

    MoveToNextFrame();
}

void Graphics::ExecuteCommandList() {
    GFX_THROW_FAILED(hr, m_pCommandList->Close());

    ID3D12CommandList* ppCommandLists[] = { m_pCommandList.Get() };
    m_pCommandQueue->ExecuteCommandLists(
        static_cast<std::uint32_t>(std::size(ppCommandLists)), ppCommandLists
    );
}

void Graphics::InitialGPUSetup() {
    ExecuteCommandList();
    m_SRVHeapMan->ProcessRequests();
    WaitForGPU();
}

void Graphics::EnableImGui() noexcept {
    m_imGuiEnabled = true;
}

void Graphics::DisableImGui() noexcept {
    m_imGuiEnabled = false;
}

bool Graphics::IsImGuiEnabled() const noexcept {
    return m_imGuiEnabled;
}

void Graphics::ImGuiBegin() {
    ImGui_ImplDX12_GetSRVHeapData();
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void Graphics::ImGuiEnd() {
	ImGui::Render();
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), m_pCommandList.Get());

    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault(nullptr, (void*)m_pCommandList.Get());
	}
}
