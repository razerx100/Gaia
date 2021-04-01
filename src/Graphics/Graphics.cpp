#include <Graphics.hpp>
#include <sstream>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <GraphicsThrowMacros.hpp>

struct Vertex
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT4 color;
};

// Graphics
Graphics::Graphics(HWND hwnd, std::uint32_t width, std::uint32_t height)
	: m_pDevice(nullptr),
	m_pSwapChain(nullptr),
	m_width(width), m_height(height), m_color{0.0f, 0.0f, 0.0f, 1.0f},
	m_FenceValues{}, m_RTVHeapSize(0), m_CurrentBackBufferIndex(0),
    m_Viewport{ 0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height) },
    m_ScissorRect{0, 0, static_cast<long>(width), static_cast<long>(height)},
    m_triangleIndicesCount(0u) {

	SetShaderPath();

    LoadPipeline(hwnd);
    LoadTriangle();
}

Graphics::~Graphics() {
	WaitForGPU();

	CloseHandle(m_FenceEvent);
}

void Graphics::LoadPipeline(HWND hwnd) {
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

    {
        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
        rtvHeapDesc.NumDescriptors = bufferCount;
        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

        GFX_THROW_FAILED(hr, m_pDevice->CreateDescriptorHeap(
            &rtvHeapDesc, __uuidof(ID3D12DescriptorHeap), &m_pRTVHeap
        ));

        m_RTVHeapSize = m_pDevice->GetDescriptorHandleIncrementSize(
            D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    }

    {
        D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
        dsvHeapDesc.NumDescriptors = bufferCount;
        dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

        GFX_THROW_FAILED(hr, m_pDevice->CreateDescriptorHeap(
            &dsvHeapDesc, __uuidof(ID3D12DescriptorHeap), &m_pDSVHeap
        ));
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
            rtvHandle.Offset(1, m_RTVHeapSize);

            GFX_THROW_FAILED(hr, m_pDevice->CreateCommandAllocator(
                D3D12_COMMAND_LIST_TYPE_DIRECT,
                __uuidof(ID3D12CommandAllocator),
                &m_pCommandAllocators[n]
            ));
        }
    }

    GFX_THROW_FAILED(hr, m_pDevice->CreateCommandList(
        0, D3D12_COMMAND_LIST_TYPE_DIRECT,
        m_pCommandAllocators[m_CurrentBackBufferIndex].Get(),
        nullptr,
        __uuidof(ID3D12CommandList),
        &m_pCommandList
    ));

    GFX_THROW_FAILED(hr, m_pCommandList->Close());

    {
        CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
        rootSignatureDesc.Init(0,
            nullptr, 0, nullptr,
            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

        ComPtr<ID3DBlob> signature;
        ComPtr<ID3DBlob> error;
        GFX_THROW_FAILED(hr, D3D12SerializeRootSignature(
            &rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error)
        );
        GFX_THROW_FAILED(hr, m_pDevice->CreateRootSignature(
            0, signature->GetBufferPointer(),
            signature->GetBufferSize(),
            __uuidof(ID3D12RootSignature),
            &m_pRootSignature
        ));
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

    WaitForGPU();
}

void Graphics::LoadTriangle() {
    {
        ComPtr<ID3DBlob> vertexShader;
        ComPtr<ID3DBlob> pixelShader;

        GFX_THROW_FAILED(hr, D3DReadFileToBlob(
            (m_ShaderPath + L"TVertexShader.cso").c_str(), &vertexShader));
        GFX_THROW_FAILED(hr, D3DReadFileToBlob(
            (m_ShaderPath + L"TPixelShader.cso").c_str(), &pixelShader));

        D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
            0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
            12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
        };

        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.InputLayout = {
            inputElementDescs, static_cast<std::uint32_t>(std::size(inputElementDescs))
        };
        psoDesc.pRootSignature = m_pRootSignature.Get();
        psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
        psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
        psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        psoDesc.DepthStencilState.DepthEnable = FALSE;
        psoDesc.DepthStencilState.StencilEnable = FALSE;
        psoDesc.SampleMask = UINT_MAX;
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = DXGI_FORMAT_B8G8R8A8_UNORM;
        psoDesc.SampleDesc.Count = 1;
        GFX_THROW_FAILED(hr, m_pDevice->CreateGraphicsPipelineState(
            &psoDesc, __uuidof(ID3D12PipelineState), &m_pPipelineState
        ));
    }

    {
        Vertex triangleVertices[] =
        {
            { { -0.35f, 0.35f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
            { { 0.35f, 0.35f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
            { { -0.35f, -0.35f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
            { { 0.35f, -0.35f, 0.0f }, { 1.0f, 0.0f, 1.0f, 1.0f } }
        };

        const std::uint32_t vertexBufferSize = sizeof(triangleVertices);
        CD3DX12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

        CD3DX12_RESOURCE_DESC rcDesc = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);
        GFX_THROW_FAILED(hr, m_pDevice->CreateCommittedResource(
            &heapProp,
            D3D12_HEAP_FLAG_NONE,
            &rcDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            __uuidof(ID3D12Resource),
            &m_pVertexBuffer
        ));

        UINT8* pVertexDataBegin;
        CD3DX12_RANGE readRange(0, 0);
        GFX_THROW_FAILED(hr, m_pVertexBuffer->Map(
            0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin)));
        memcpy(pVertexDataBegin, triangleVertices, sizeof(triangleVertices));
        m_pVertexBuffer->Unmap(0, nullptr);

        m_VertexBufferView.BufferLocation = m_pVertexBuffer->GetGPUVirtualAddress();
        m_VertexBufferView.StrideInBytes = sizeof(Vertex);
        m_VertexBufferView.SizeInBytes = vertexBufferSize;
    }

    {
        std::uint16_t triangleIndices[] =
        {
            0u, 1u, 2u, 2u, 1u, 3u
        };

        m_triangleIndicesCount = static_cast<std::uint32_t>(std::size(triangleIndices));

        const std::uint32_t indexBufferSize = sizeof(triangleIndices);
        CD3DX12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

        CD3DX12_RESOURCE_DESC rcDesc = CD3DX12_RESOURCE_DESC::Buffer(indexBufferSize);
        GFX_THROW_FAILED(hr, m_pDevice->CreateCommittedResource(
            &heapProp,
            D3D12_HEAP_FLAG_NONE,
            &rcDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            __uuidof(ID3D12Resource),
            &m_pIndexBuffer
        ));

        UINT8* pIndexDataBegin;
        CD3DX12_RANGE readRange(0, 0);
        GFX_THROW_FAILED(hr, m_pIndexBuffer->Map(
            0, &readRange, reinterpret_cast<void**>(&pIndexDataBegin)));
        memcpy(pIndexDataBegin, triangleIndices, indexBufferSize);
        m_pIndexBuffer->Unmap(0, nullptr);

        m_IndexBufferView.BufferLocation = m_pIndexBuffer->GetGPUVirtualAddress();
        m_IndexBufferView.SizeInBytes = indexBufferSize;
        m_IndexBufferView.Format = DXGI_FORMAT_R16_UINT;
    }
}

void Graphics::ClearBuffer(float red, float green, float blue) {
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(
        m_pRTVHeap->GetCPUDescriptorHandleForHeapStart(),
        m_CurrentBackBufferIndex, m_RTVHeapSize);

    m_color[0] = red;
    m_color[1] = green;
    m_color[2] = blue;

    m_pCommandList->ClearRenderTargetView(rtvHandle, m_color, 0, nullptr);
}

void Graphics::DrawTriangle() {
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(
        m_pRTVHeap->GetCPUDescriptorHandleForHeapStart(),
        m_CurrentBackBufferIndex, m_RTVHeapSize);

    m_pCommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

    m_pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_pCommandList->IASetVertexBuffers(0, 1, &m_VertexBufferView);
    m_pCommandList->IASetIndexBuffer(&m_IndexBufferView);
    m_pCommandList->DrawIndexedInstanced(m_triangleIndicesCount, 1u, 0u, 0u, 0u);
}

void Graphics::ResetCommandList() {
    GFX_THROW_FAILED(hr, m_pCommandAllocators[m_CurrentBackBufferIndex]->Reset());

    GFX_THROW_FAILED(hr, m_pCommandList->Reset(
        m_pCommandAllocators[m_CurrentBackBufferIndex].Get(), m_pPipelineState.Get()));

    m_pCommandList->SetGraphicsRootSignature(m_pRootSignature.Get());
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
    CD3DX12_RESOURCE_BARRIER postbar = CD3DX12_RESOURCE_BARRIER::Transition(
        m_pRenderTargets[m_CurrentBackBufferIndex].Get(),
        D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

    m_pCommandList->ResourceBarrier(1, &postbar);

    GFX_THROW_FAILED(hr, m_pCommandList->Close());

    ID3D12CommandList* ppCommandLists[] = { m_pCommandList.Get() };
    m_pCommandQueue->ExecuteCommandLists(
        static_cast<std::uint32_t>(std::size(ppCommandLists)), ppCommandLists
    );

    GFX_THROW_FAILED(hr, m_pSwapChain->Present(0, DXGI_PRESENT_ALLOW_TEARING));

    MoveToNextFrame();
}

// Utility
void Graphics::SetShaderPath() noexcept {
	wchar_t path[MAX_PATH];
	GetModuleFileNameW(nullptr, path, MAX_PATH);
	m_ShaderPath = path;
	for (int i = static_cast<int>(m_ShaderPath.size() - 1); m_ShaderPath[i] != L'\\'; i--)
		m_ShaderPath.pop_back();

	m_ShaderPath.append(L"shaders\\");
}
