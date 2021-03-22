#include <Graphics.hpp>
#include <sstream>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <GraphicsThrowMacros.hpp>

// Graphics
Graphics::Graphics(HWND hwnd, std::uint32_t width, std::uint32_t height)
	: m_pDevice(nullptr),
	m_pSwapChain(nullptr),
	m_pDeviceContext(nullptr),
	m_pTargetView(nullptr),
	m_width(width), m_height(height) {

	SetShaderPath();

	DXGI_SWAP_CHAIN_DESC desc = { };
	desc.BufferDesc.Width = 0;
	desc.BufferDesc.Height = 0;
	desc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	desc.BufferDesc.RefreshRate.Numerator = 0;
	desc.BufferDesc.RefreshRate.Denominator = 0;
	desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.BufferCount = 2;
	desc.OutputWindow = hwnd;
	desc.Windowed = TRUE;
	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	desc.Flags = 0;

	HRESULT hr;
	UINT swapCreateFlag = 0u;

#ifdef _DEBUG
	swapCreateFlag |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	GFX_THROW_FAILED(hr, D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		swapCreateFlag,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&desc,
		&m_pSwapChain,
		&m_pDevice,
		nullptr,
		&m_pDeviceContext
	));

	D3D11_VIEWPORT vp = {};
	vp.Width = static_cast<float>(m_width);
	vp.Height =static_cast<float>(m_height);
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;

	m_pDeviceContext->RSSetViewports(1u, &vp);

	ComPtr<ID3D11Resource> pBackBuffer = nullptr;
	GFX_THROW_FAILED(hr, m_pSwapChain->GetBuffer(
		0u,
		__uuidof(ID3D11Resource),
		&pBackBuffer
	));

	GFX_THROW_FAILED(hr, m_pDevice->CreateRenderTargetView(
		pBackBuffer.Get(), nullptr, &m_pTargetView
	));

	// Depth Stencil
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
	);

	m_pDeviceContext->OMSetRenderTargets(
		1u, m_pTargetView.GetAddressOf(), m_pDepthStencilView.Get()
		);
}

void Graphics::EndFrame() {
	HRESULT hr;

#ifdef _DEBUG
	DXGI_INFO_MAN.Set();
#endif
	if (FAILED(hr = m_pSwapChain->Present(0u, 0u))) {
		if (hr == DXGI_ERROR_DEVICE_REMOVED)
			throw GFX_DEVICE_REMOVED_EXCEPT(m_pDevice->GetDeviceRemovedReason());
		else
			GFX_THROW(hr);
	}

	m_pDeviceContext->OMSetRenderTargets(
		1u, m_pTargetView.GetAddressOf(), m_pDepthStencilView.Get()
	);
}

void Graphics::ClearBuffer(float red, float green, float blue) noexcept {
	const float color[] = { red, green, blue, 1.0f };
	m_pDeviceContext->ClearRenderTargetView(m_pTargetView.Get(), color);
	m_pDeviceContext->ClearDepthStencilView(
		m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u
	);
}

void Graphics::DrawTriangle(float angle, float posX, float posY) {
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
	ComPtr<ID3D11Buffer> pVertexBuffer;
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

