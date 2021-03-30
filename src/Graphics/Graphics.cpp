#include <Graphics.hpp>
#include <sstream>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <GraphicsThrowMacros.hpp>
#include <d3dx12.h>

// Graphics
Graphics::Graphics(HWND hwnd, std::uint32_t width, std::uint32_t height)
	: m_pDevice(nullptr),
	m_pSwapChain(nullptr),
	m_width(width), m_height(height) {

	SetShaderPath();

	m_ViewPort.Width = static_cast<float>(m_width);
	m_ViewPort.Height = static_cast<float>(m_height);
	m_ViewPort.MaxDepth = 1;

	m_ScissorRect.right = static_cast<long>(m_width);
	m_ScissorRect.bottom = static_cast<long>(m_height);

	UINT factoryCreateFlag = 0u;

#ifdef _DEBUG
	{
		ComPtr<ID3D12Debug> debugController;
		D3D12GetDebugInterface(__uuidof(ID3D12Debug), &debugController);
		debugController->EnableDebugLayer();

		factoryCreateFlag |= DXGI_CREATE_FACTORY_DEBUG;
	}
#endif

	ComPtr<IDXGIFactory4> pfactory;

	GFX_THROW_FAILED(hr,
		CreateDXGIFactory2(factoryCreateFlag, __uuidof(IDXGIFactory4), &pfactory)
	);

	ComPtr<IDXGIAdapter1> pAdapter;

	GFX_THROW_FAILED(hr, pfactory->EnumAdapters1(
		0u, &pAdapter
	));

	ComPtr<IDXGIAdapter4> pAdapter4;
	GFX_THROW_FAILED(hr, pAdapter.As(&pAdapter4));

	GFX_THROW_FAILED(hr, D3D12CreateDevice(
		pAdapter4.Get(),
		D3D_FEATURE_LEVEL_11_0,
		__uuidof(ID3D12Device2),
		&m_pDevice
	));

	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	GFX_THROW_FAILED(hr, m_pDevice->CreateCommandQueue(
		&queueDesc, __uuidof(ID3D12CommandQueue), &m_pCommandQueue
	));

	DXGI_SWAP_CHAIN_DESC1 desc = { };
	desc.Width = 0;
	desc.Height = 0;
	desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.BufferCount = bufferCount;
	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

	ComPtr<IDXGISwapChain1> pSwapChain;

	GFX_THROW_FAILED(hr, pfactory->CreateSwapChainForHwnd(
		m_pCommandQueue.Get(),
		hwnd,
		&desc,
		nullptr,
		nullptr,
		&pSwapChain
	));

	GFX_THROW_FAILED(hr, pSwapChain.As(&m_pSwapChain));

	m_CurrentBackBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();

	D3D12_DESCRIPTOR_HEAP_DESC rtvDesc = {};
	rtvDesc.NumDescriptors = bufferCount;
	rtvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	GFX_THROW_FAILED(hr, m_pDevice->CreateDescriptorHeap(
		&rtvDesc, __uuidof(ID3D12DescriptorHeap), &m_pRTVHeap
	));

	m_RTVSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	D3D12_DESCRIPTOR_HEAP_DESC srvDesc = {};
	rtvDesc.NumDescriptors = 1;
	rtvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	rtvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	GFX_THROW_FAILED(hr, m_pDevice->CreateDescriptorHeap(
		&srvDesc, __uuidof(ID3D12DescriptorHeap), &m_pSRVHeap
	));

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtv_handle(m_pRTVHeap->GetCPUDescriptorHandleForHeapStart());

	for (std::uint32_t n = 0u; n < bufferCount; n++) {
		GFX_THROW_FAILED(hr,
			m_pSwapChain->GetBuffer(n, __uuidof(ID3D12Resource), &m_pRenderTargets[n])
		);
		GFX_THROW_NO_HR(m_pDevice->CreateRenderTargetView(
			m_pRenderTargets[n].Get(),
			nullptr,
			rtv_handle
		));

		rtv_handle.Offset(1, m_RTVSize);

		GFX_THROW_FAILED(hr,
			m_pDevice->CreateCommandAllocator(
					D3D12_COMMAND_LIST_TYPE_DIRECT,
					__uuidof(ID3D12CommandAllocator),
					&m_pCommandAllocator[n]
				)
			);
	}

	/*// Depth Stencil
	D3D11_DEPTH_STENCIL_DESC sStateDesc = {};
	sStateDesc.DepthEnable = true;
	sStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	sStateDesc.DepthFunc = D3D11_COMPARISON_LESS;

	ComPtr<ID3D11DepthStencilState> pDsState;

	m_pDevice->CreateDepthStencilState(&sStateDesc, &pDsState);

	m_pDeviceContext->OMSetDepthStencilState(pDsState.Get(), 1u);

	ComPtr<ID3D11Texture2D> pDepthTexture;
	D3D11_TEXTURE2D_DESC descDepth = {};
	descDepth.Width = m_width;
	descDepth.Height = m_height;
	descDepth.MipLevels = 1u;
	descDepth.ArraySize = 1u;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1u;
	descDepth.SampleDesc.Quality = 0u;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	m_pDevice->CreateTexture2D(&descDepth, nullptr, &pDepthTexture);

	D3D11_DEPTH_STENCIL_VIEW_DESC sViewDesc = {};
	sViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	sViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	sViewDesc.Texture2D.MipSlice = 0u;

	GFX_THROW_FAILED(hr,
		m_pDevice->CreateDepthStencilView(
			pDepthTexture.Get(), &sViewDesc, &m_pDepthStencilView)
	);*/

	GFX_THROW_FAILED(hr, m_pDevice->CreateCommandList(
		0, D3D12_COMMAND_LIST_TYPE_DIRECT,
		m_pCommandAllocator[m_CurrentBackBufferIndex].Get(),
		nullptr,
		__uuidof(ID3D12CommandList),
		&m_pCommandList
	));

	GFX_THROW_FAILED(hr, m_pCommandList->Close());

	GFX_THROW_FAILED(hr,
		m_pDevice->CreateFence(
			m_FenceValues[m_CurrentBackBufferIndex],
			D3D12_FENCE_FLAG_NONE,
			__uuidof(ID3D12Fence),
			&m_Fence
		)
	);
	m_FenceValues[m_CurrentBackBufferIndex]++;

	m_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

	WaitForGPU();
}

Graphics::~Graphics() {
	WaitForGPU();

	CloseHandle(m_FenceEvent);
}

void Graphics::EndFrame() {
#ifdef _DEBUG
	DXGI_INFO_MAN.Set();
#endif
	ID3D12CommandList* ppCommandLists[] = { m_pCommandList.Get() };
	m_pCommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	if (FAILED(hr = m_pSwapChain->Present(0u, DXGI_PRESENT_ALLOW_TEARING))) {
		if (hr == DXGI_ERROR_DEVICE_REMOVED)
			throw GFX_DEVICE_REMOVED_EXCEPT(m_pDevice->GetDeviceRemovedReason());
		else
			GFX_THROW(hr);
	}

	MoveToNextFrame();
}

void Graphics::ClearBuffer(float red, float green, float blue) {
	const float color[] = { red, green, blue, 1.0f };
	GFX_THROW_FAILED(hr, m_pCommandAllocator[m_CurrentBackBufferIndex]->Reset());

	GFX_THROW_FAILED(hr, m_pCommandList->Reset(
		m_pCommandAllocator[m_CurrentBackBufferIndex].Get(),
		nullptr
	));

	m_pCommandList->RSSetScissorRects(1, &m_ScissorRect);
	m_pCommandList->RSSetViewports(1, &m_ViewPort);

	D3D12_RESOURCE_BARRIER rcBarrier = {};
	rcBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	rcBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	rcBarrier.Transition.pResource = m_pRenderTargets[m_CurrentBackBufferIndex].Get();
	rcBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	rcBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

	m_pCommandList->ResourceBarrier(1, &rcBarrier);


	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(
		m_pRTVHeap->GetCPUDescriptorHandleForHeapStart(),
		m_CurrentBackBufferIndex, m_RTVSize
	);

	m_pCommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

	m_pCommandList->ClearRenderTargetView(rtvHandle, color, 0, nullptr);
	/*m_pDeviceContext->ClearDepthStencilView(
		m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u
	);*/

	rcBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	rcBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;

	m_pCommandList->ResourceBarrier(1, &rcBarrier);

	m_pCommandList->Close();
}

/*void Graphics::DrawTriangle(float angle, float posX, float posY) {
	struct Position {
		float x;
		float y;
		float z;
	};

	struct Vertex {
		Position position;
	};

	const Vertex vertices[] = {
		{-1.0f, -1.0f, -1.0f},
		{1.0f, -1.0f, -1.0f},
		{-1.0f, 1.0f, -1.0f},
		{1.0f, 1.0f, -1.0f},
		{-1.0f, -1.0f, 1.0f},
		{1.0f, -1.0f, 1.0f},
		{-1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},
	};

	m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	const std::uint32_t stride = sizeof(Vertex);


	// Vertex Buffer
	ComPtr<ID3D12Resource> pVertexBuffer;

	D3D11_BUFFER_DESC vertexDesc = {};
	vertexDesc.ByteWidth = sizeof(vertices);
	vertexDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexDesc.CPUAccessFlags = 0u;
	vertexDesc.MiscFlags = 0u;
	vertexDesc.StructureByteStride = stride;

	D3D11_SUBRESOURCE_DATA vertexData = {};
	vertexData.pSysMem = vertices;

	HRESULT hr;

	GFX_THROW_FAILED(hr, m_pDevice->CreateBuffer(&vertexDesc, &vertexData, &pVertexBuffer));

	const std::uint32_t offSet = 0u;
	m_pDeviceContext->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offSet);

	// Indices
	const unsigned short indices[] = {
		0u, 2u, 1u,		2u, 3u, 1u,
		1u, 3u, 5u,		3u, 7u, 5u,
		2u, 6u, 3u,		6u, 7u, 3u,
		4u, 5u, 7u,		4u, 7u, 6u,
		0u, 4u, 2u,		2u, 4u, 6u,
		0u, 1u, 4u,		1u, 5u, 4u
	};

	// Index Buffer
	ComPtr<ID3D11Buffer> pIndexBuffer;
	D3D11_BUFFER_DESC indexDesc;
	indexDesc.ByteWidth = sizeof(indices);
	indexDesc.Usage = D3D11_USAGE_DEFAULT;
	indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexDesc.CPUAccessFlags = 0u;
	indexDesc.MiscFlags = 0u;
	indexDesc.StructureByteStride = sizeof(unsigned short);

	D3D11_SUBRESOURCE_DATA indexData = {};
	indexData.pSysMem = indices;

	GFX_THROW_FAILED(hr, m_pDevice->CreateBuffer(&indexDesc, &indexData, &pIndexBuffer));

	m_pDeviceContext->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);

	// Vertex Shader
	ComPtr<ID3D11VertexShader> pVertexShader;
	ComPtr<ID3DBlob> pBlob;
	GFX_THROW_FAILED(hr, D3DReadFileToBlob(
		(m_ShaderPath + L"TVertexShader.cso").c_str(), &pBlob));
	GFX_THROW_FAILED(hr, m_pDevice->CreateVertexShader(
		pBlob->GetBufferPointer(), pBlob->GetBufferSize(),
		nullptr, &pVertexShader
	));

	m_pDeviceContext->VSSetShader(pVertexShader.Get(), nullptr, 0u);

	// Vertex Layout
	ComPtr<ID3D11InputLayout> pInputLayout;

	const D3D11_INPUT_ELEMENT_DESC inputDescs[] = {
		{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	GFX_THROW_FAILED(hr, m_pDevice->CreateInputLayout(
		inputDescs, static_cast<std::uint32_t>(std::size(inputDescs)),
		pBlob->GetBufferPointer(),
		pBlob->GetBufferSize(),
		&pInputLayout
	));

	m_pDeviceContext->IASetInputLayout(pInputLayout.Get());

	// Constant Buffer Vertex Transform
	struct ConstantBufferTransform {
		DirectX::XMMATRIX transform;
	};

	const ConstantBufferTransform constBufferT = {
		{
			DirectX::XMMatrixRotationX(angle) *
			DirectX::XMMatrixRotationY(angle) *
			DirectX::XMMatrixRotationZ(angle) *
			DirectX::XMMatrixTranslation(posX, posY, 4.0f) *
			DirectX::XMMatrixPerspectiveLH(1.0f, 9.0f / 16.0f, 0.5f, 5.0f)
		}
	};

	ComPtr<ID3D11Buffer> pConstantBufferTransform;
	D3D11_BUFFER_DESC transformDesc;
	transformDesc.ByteWidth = sizeof(constBufferT);
	transformDesc.Usage = D3D11_USAGE_DYNAMIC;
	transformDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	transformDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	transformDesc.MiscFlags = 0u;
	transformDesc.StructureByteStride = 0u;

	D3D11_SUBRESOURCE_DATA transformData = {};
	transformData.pSysMem = &constBufferT;

	GFX_THROW_FAILED(hr, m_pDevice->CreateBuffer(&transformDesc, &transformData , &pConstantBufferTransform));

	m_pDeviceContext->VSSetConstantBuffers(0u, 1u, pConstantBufferTransform.GetAddressOf());

	// Pixel Shader
	ComPtr<ID3D11PixelShader> pPixelShader;
	GFX_THROW_FAILED(hr, D3DReadFileToBlob(
		(m_ShaderPath + L"TPixelShader.cso").c_str(), &pBlob));
	GFX_THROW_FAILED(hr, m_pDevice->CreatePixelShader(
		pBlob->GetBufferPointer(), pBlob->GetBufferSize(),
		nullptr, &pPixelShader
	));

	m_pDeviceContext->PSSetShader(pPixelShader.Get(), nullptr, 0u);

	// Constant Buffer Face colors
	struct Color {
		float red;
		float green;
		float blue;
		float alpha;
	};

	struct ConstantBufferColor {
		Color color;
	};

	const ConstantBufferColor constBufferC[6] = {
		{1.0f, 0.0f, 0.0f, 1.0f},
		{0.0f, 1.0f, 1.0f, 1.0f},
		{0.0f, 1.0f, 0.0f, 1.0f},
		{0.0f, 0.0f, 1.0f, 1.0f},
		{1.0f, 0.0f, 1.0f, 1.0f},
		{0.0f, 0.75f, 0.5f, 1.0f}
	};

	ComPtr<ID3D11Buffer> pConstantBufferColor;
	D3D11_BUFFER_DESC colorDesc;
	colorDesc.ByteWidth = sizeof(constBufferC);
	colorDesc.Usage = D3D11_USAGE_DYNAMIC;
	colorDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	colorDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	colorDesc.MiscFlags = 0u;
	colorDesc.StructureByteStride = 0u;

	D3D11_SUBRESOURCE_DATA colorData = {};
	colorData.pSysMem = &constBufferC;

	GFX_THROW_FAILED(hr, m_pDevice->CreateBuffer(&colorDesc, &colorData , &pConstantBufferColor));

	m_pDeviceContext->PSSetConstantBuffers(0u, 1u, pConstantBufferColor.GetAddressOf());

	GFX_THROW_NO_HR(m_pDeviceContext->DrawIndexed(static_cast<std::uint32_t>(std::size(indices)), 0u, 0u));
}*/

void Graphics::WaitForGPU() {
	GFX_THROW_FAILED(hr, m_pCommandQueue->Signal(
		m_Fence.Get(), m_FenceValues[m_CurrentBackBufferIndex]
	));

	GFX_THROW_FAILED(hr, m_Fence->SetEventOnCompletion(
		m_FenceValues[m_CurrentBackBufferIndex], m_FenceEvent
	));
	WaitForSingleObjectEx(m_FenceEvent, INFINITE, FALSE);

	m_FenceValues[m_CurrentBackBufferIndex]++;
}

void Graphics::MoveToNextFrame() {
	const std::uint64_t currentFenceValue = m_FenceValues[m_CurrentBackBufferIndex];
	GFX_THROW_FAILED(hr, m_pCommandQueue->Signal(
		m_Fence.Get(), currentFenceValue
	));

	m_CurrentBackBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();

	if (m_Fence->GetCompletedValue() < m_FenceValues[m_CurrentBackBufferIndex]) {
		GFX_THROW_FAILED(hr, m_Fence->SetEventOnCompletion(
			m_FenceValues[m_CurrentBackBufferIndex], m_FenceEvent
		));
		WaitForSingleObjectEx(m_FenceEvent, INFINITE, FALSE);
	}

	m_FenceValues[m_CurrentBackBufferIndex] = currentFenceValue + 1;
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

