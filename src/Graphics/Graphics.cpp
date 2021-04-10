#include <Graphics.hpp>
#include <DirectXMath.h>
#include <GraphicsThrowMacros.hpp>
#include <ConstantBuffers.hpp>
#include <Shaders.hpp>
#include <VertexLayout.hpp>
#include <Topology.hpp>
#include <VertexBuffer.hpp>
#include <IndexBuffer.hpp>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

// Graphics
Graphics::Graphics(HWND hwnd, std::uint32_t width, std::uint32_t height)
	: m_pDevice(nullptr),
	m_pSwapChain(nullptr),
	m_pDeviceContext(nullptr),
	m_pTargetView(nullptr),
	m_width(width), m_height(height), m_imGuiEnabled(true), hr(0) {

	ComPtr<IDXGIFactory> pFactory;
	GFX_THROW_FAILED(hr, CreateDXGIFactory(
		__uuidof(pFactory.Get()),
		&pFactory
	));

	UINT createDeviceFlags = 0u;

#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	GFX_THROW_FAILED(hr, D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		createDeviceFlags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&m_pDevice,
		nullptr,
		&m_pDeviceContext
	));

	// ImGui
	ImGui_ImplDX11_Init(m_pDevice.Get(), m_pDeviceContext.Get());
	// ImGui

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
	desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

	GFX_THROW_FAILED(hr, pFactory->CreateSwapChain(
		m_pDevice.Get(),
		&desc,
		&m_pSwapChain
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

Graphics::~Graphics() {
	ImGui_ImplDX11_Shutdown();
}

void Graphics::EndFrame() {

	ImGuiEnd();

#ifdef _DEBUG
	DXGI_INFO_MAN.Set();
#endif
	if (FAILED(hr = m_pSwapChain->Present(0u, DXGI_PRESENT_ALLOW_TEARING))) {
		if (hr == DXGI_ERROR_DEVICE_REMOVED)
			throw GFX_DEVICE_REMOVED_EXCEPT(m_pDevice->GetDeviceRemovedReason());
		else
			GFX_THROW(hr);
	}

	m_pDeviceContext->OMSetRenderTargets(
		1u, m_pTargetView.GetAddressOf(), m_pDepthStencilView.Get()
	);
}

void Graphics::BeginFrame(float red, float green, float blue) noexcept {
	ImGuiBegin();

	const float color[] = { red, green, blue, 1.0f };
	m_pDeviceContext->ClearRenderTargetView(m_pTargetView.Get(), color);
	m_pDeviceContext->ClearDepthStencilView(
		m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u
	);
}

void Graphics::DrawIndexed(std::uint32_t count) noexcept(!IS_DEBUG) {
	GFX_THROW_NO_HR(m_pDeviceContext->DrawIndexed(count, 0u, 0u));
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
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void Graphics::ImGuiEnd() {
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}
