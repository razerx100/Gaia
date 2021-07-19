#include <Graphics.hpp>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <GraphicsThrowMacros.hpp>
#include <ImGuiImpl.hpp>
#include <BufferMan.hpp>
#include <Camera.hpp>

DXGI_FORMAT Graphics::s_renderFormat;
// Graphics
Graphics::Graphics(HWND hwnd, std::uint32_t width, std::uint32_t height)
	: m_pDevice(nullptr),
	m_pSwapChain(nullptr),
	m_width(width), m_height(height),
	m_fenceValues{0, 0}, m_nRTVHeapIncSize(0), m_currentBackBufferIndex(0),
    m_color{0.0f, 0.0f, 0.0f, 1.0f},
    hr(0) {

    InitializeViewPortAndSRECT();
    Initialize(hwnd);
    ImGuiImpl::ImGuiDxInit(
        m_pDevice.Get(), bufferCount, GetRenderFormat(), m_pSRVHeapMan.get()
    );
}

Graphics::~Graphics() {
	WaitForGPU();

	CloseHandle(m_fenceEvent);

    ImGuiImpl::ImGuiDxQuit();
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

        GetHardwareAdapter(factory.Get(), &adapter);

        D3D12CreateDevice(
            adapter.Get(),
            D3D_FEATURE_LEVEL_11_0,
            __uuidof(ID3D12Device5),
            &m_pDevice
        );
    }

#ifdef _DEBUG
    DebugInfoManager::SetDebugInfoManager(m_pDevice.Get());
#endif

    BufferMan::Init(this);

    m_gfxCommandQueue.Init(*this, D3D12_COMMAND_LIST_TYPE_DIRECT, bufferCount);
    m_gfxCommandListRef = m_gfxCommandQueue.GetCommandList(0u);
    m_gfxCommandListRef->Record();

    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.BufferCount = bufferCount;
    swapChainDesc.Width = m_width;
    swapChainDesc.Height = m_height;
    swapChainDesc.Format = GetRenderFormat();
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

    ComPtr<IDXGISwapChain1> swapChain;
    GFX_THROW_FAILED(hr, factory->CreateSwapChainForHwnd(
        m_gfxCommandQueue.GetCommandQueue(),
        hwnd,
        &swapChainDesc,
        nullptr,
        nullptr,
        &swapChain
    ));

    factory->MakeWindowAssociation(
        hwnd,
        DXGI_MWA_NO_ALT_ENTER
    );

    GFX_THROW_FAILED(hr, swapChain.As(&m_pSwapChain));
    m_currentBackBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();

    // RTV HEAP
    {
        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
        rtvHeapDesc.NumDescriptors = bufferCount;
        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

        GFX_THROW_FAILED(hr, m_pDevice->CreateDescriptorHeap(
            &rtvHeapDesc, __uuidof(ID3D12DescriptorHeap), &m_pRTVHeap
        ));

        m_nRTVHeapIncSize = m_pDevice->GetDescriptorHandleIncrementSize(
            D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    }

    CreateRTVs();

    // DSV HEAP
    {
        D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
        dsvHeapDesc.NumDescriptors = 1;
        dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

        GFX_THROW_FAILED(hr, m_pDevice->CreateDescriptorHeap(
            &dsvHeapDesc, __uuidof(ID3D12DescriptorHeap), &m_pDSVHeap
        ));
    }

    CreateDSV();

    GFX_THROW_FAILED(hr, m_pDevice->CreateFence(
        m_fenceValues[m_currentBackBufferIndex],
        D3D12_FENCE_FLAG_NONE,
        __uuidof(ID3D12Fence),
        &m_pFence
    ));
    m_fenceValues[m_currentBackBufferIndex]++;

    m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (m_fenceEvent == nullptr)
        GFX_THROW_FAILED(hr, HRESULT_FROM_WIN32(GetLastError()));

    // Command List
    m_pSRVHeapMan = std::make_unique<HeapMan>(
        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, this
        );
}

void Graphics::BeginFrame() {
    m_gfxCommandListRef->Record();
    TransitionIntoRenderMode(
        m_pRenderTargets[m_currentBackBufferIndex].Get(),
        m_gfxCommandListRef->Get()
    );

    RecordCommonCommands(m_gfxCommandListRef->Get());
}

void Graphics::SetBGColor(float red, float green, float blue) {
    m_color[0] = red;
    m_color[1] = green;
    m_color[2] = blue;
}

void Graphics::RecordCommonCommands(ID3D12GraphicsCommandList* commandList) {
    ID3D12DescriptorHeap* ppHeaps[] = {
        m_pSRVHeapMan->GetHeap()
    };

    commandList->SetDescriptorHeaps(
        m_pSRVHeapMan->GetHeapCount(), ppHeaps
    );

    commandList->RSSetViewports(1, &m_viewport);
    commandList->RSSetScissorRects(1, &m_scissorRect);

    ImGuiImpl::ImGuiBeginFrame();

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(
        m_pRTVHeap->GetCPUDescriptorHandleForHeapStart(),
        m_currentBackBufferIndex, m_nRTVHeapIncSize);

    commandList->ClearRenderTargetView(rtvHandle, m_color, 0, nullptr);

    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle =
        m_pDSVHeap->GetCPUDescriptorHandleForHeapStart();

    commandList->ClearDepthStencilView(
        dsvHandle,
        D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

    commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);
}

void Graphics::DrawIndexed(
    ID3D12GraphicsCommandList* commandList,
    std::uint32_t indexCount
) noexcept {
    commandList->DrawIndexedInstanced(indexCount, 1u, 0u, 0u, 0u);
}

void Graphics::WaitForGPU() {
    GFX_THROW_FAILED(hr, m_gfxCommandQueue.GetCommandQueue()->Signal(
        m_pFence.Get(), m_fenceValues[m_currentBackBufferIndex]));

    GFX_THROW_FAILED(hr,
        m_pFence->SetEventOnCompletion(
            m_fenceValues[m_currentBackBufferIndex], m_fenceEvent));
    WaitForSingleObjectEx(m_fenceEvent, INFINITE, FALSE);

    m_fenceValues[m_currentBackBufferIndex]++;
}

void Graphics::MoveToNextFrame() {
    const std::uint64_t currentFenceValue = m_fenceValues[m_currentBackBufferIndex];
    GFX_THROW_FAILED(hr,
        m_gfxCommandQueue.GetCommandQueue()->Signal(m_pFence.Get(),
            currentFenceValue)
    );

    m_currentBackBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();

    if (m_pFence->GetCompletedValue() < m_fenceValues[m_currentBackBufferIndex]) {
        GFX_THROW_FAILED(hr,
            m_pFence->SetEventOnCompletion(
                m_fenceValues[m_currentBackBufferIndex], m_fenceEvent));
        WaitForSingleObjectEx(m_fenceEvent, INFINITE, FALSE);
    }

    m_fenceValues[m_currentBackBufferIndex] = currentFenceValue + 1;
}

void Graphics::EndFrame() {
    ImGuiImpl::ImGuiEndFrame(m_gfxCommandListRef->Get());

    TransitionIntoPresentMode(
        m_pRenderTargets[m_currentBackBufferIndex].Get(),
        m_gfxCommandListRef->Get()
    );
    m_gfxCommandListRef->Close();
}

void Graphics::PresentFrame() {
    m_gfxCommandQueue.SubmitCommandList(m_gfxCommandListRef);
    m_gfxCommandQueue.ExecuteCommandLists();

    GFX_THROW_FAILED(hr, m_pSwapChain->Present(0, DXGI_PRESENT_ALLOW_TEARING));

    MoveToNextFrame();
    m_gfxCommandListRef->FinishedExecution();
    m_pSRVHeapMan->ProcessRequests();
}

void Graphics::InitialGPUSetup() {
    m_gfxCommandListRef->Close();
    m_gfxCommandQueue.SubmitCommandList(m_gfxCommandListRef);
    m_gfxCommandQueue.ExecuteCommandLists();

    m_pSRVHeapMan->ProcessRequests();

    WaitForGPU();
}

void Graphics::CreateRTVs() {
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(
        m_pRTVHeap->GetCPUDescriptorHandleForHeapStart()
    );

    for (std::uint32_t n = 0; n < bufferCount; n++) {
        GFX_THROW_FAILED(hr, m_pSwapChain->GetBuffer(
            n, __uuidof(ID3D12Resource), &m_pRenderTargets[n])
        );

        GFX_THROW_NO_HR(
            m_pDevice->CreateRenderTargetView(
                m_pRenderTargets[n].Get(), nullptr, rtvHandle
            )
        )
            rtvHandle.Offset(1, m_nRTVHeapIncSize);
    }
}

void Graphics::CreateDSV() {
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

void Graphics::TransitionIntoPresentMode(
    ID3D12Resource* rtv,
    ID3D12GraphicsCommandList* commandList
) {
    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        rtv,
        D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

    commandList->ResourceBarrier(1, &barrier);
}

void Graphics::TransitionIntoRenderMode(
    ID3D12Resource* rtv,
    ID3D12GraphicsCommandList* commandList
) {
    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        rtv,
        D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

    commandList->ResourceBarrier(1, &barrier);
}

void Graphics::InitializeViewPortAndSRECT() {
    m_viewport.TopLeftX = 0;
    m_viewport.TopLeftY = 0;
    m_viewport.Width = static_cast<float>(m_width);
    m_viewport.Height = static_cast<float>(m_height);
    m_viewport.MinDepth = 0.0f;
    m_viewport.MaxDepth = 1.0f;

    m_scissorRect.left = static_cast<LONG>(m_viewport.TopLeftX);
    m_scissorRect.right = static_cast<LONG>(m_viewport.TopLeftX + m_viewport.Width);
    m_scissorRect.top = static_cast<LONG>(m_viewport.TopLeftY);
    m_scissorRect.bottom = static_cast<LONG>(m_viewport.TopLeftY + m_viewport.Height);
}

void Graphics::ResizeBuffer(std::uint32_t width, std::uint32_t height) {
    if (width != m_width || height != m_height) {
        WaitForGPU();

        for (std::uint32_t n = 0; n < bufferCount; ++n) {
            m_pRenderTargets[n].Reset();
            m_fenceValues[n] = m_fenceValues[m_currentBackBufferIndex];
        }

        DXGI_SWAP_CHAIN_DESC desc = {};
        m_pSwapChain->GetDesc(&desc);
        GFX_THROW_FAILED(
            hr,
            m_pSwapChain->ResizeBuffers(
                bufferCount,
                width, height,
                desc.BufferDesc.Format,
                desc.Flags
            )
        );

        m_currentBackBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();

        m_width = width;
        m_height = height;

        InitializeViewPortAndSRECT();

        CreateRTVs();
        CreateDSV();

	    Camera::SetProjection(DirectX::XMMatrixPerspectiveLH(
            1.0f,  static_cast<float>(height)/ width, 0.5f, 60.0f)
        );
    }
}

DXGI_OUTPUT_DESC Graphics::GetOutputDesc() {
    ComPtr<IDXGIOutput> pOutput;
    GFX_THROW_FAILED(hr, m_pSwapChain->GetContainingOutput(&pOutput));

    DXGI_OUTPUT_DESC desc;
    GFX_THROW_FAILED(hr, pOutput->GetDesc(&desc));

    return desc;
}

void Graphics::SetRenderFormat(DXGI_FORMAT renderFormat) noexcept {
    s_renderFormat = renderFormat;
}

DXGI_FORMAT Graphics::GetRenderFormat() noexcept {
    return s_renderFormat;
}

void Graphics::GetHardwareAdapter(
    IDXGIFactory1* pFactory,
    IDXGIAdapter1** ppAdapter
) {
    ComPtr<IDXGIFactory6> pFactory6;

    if (SUCCEEDED(pFactory->QueryInterface(__uuidof(IDXGIFactory6), &pFactory6)))
        pFactory6->EnumAdapterByGpuPreference(
            0u,
            DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
            __uuidof(IDXGIAdapter1),
            reinterpret_cast<void**>(ppAdapter)
        );
    else
        pFactory->EnumAdapters1(
            0u, ppAdapter
        );

    if (FAILED(
        D3D12CreateDevice(
            *ppAdapter, D3D_FEATURE_LEVEL_11_0,
            __uuidof(ID3D12Device), nullptr)
    ))
        throw GenericException("GPU doesn't have hardware support for DirectX12.");
}
