#include <Graphics.hpp>

Graphics::Graphics(HWND hwnd)
	: m_pDevice(nullptr),
	m_pSwapChain(nullptr),
	m_pDeviceContext(nullptr),
	m_pTargetView(nullptr) {

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
	desc.BufferCount = 1;
	desc.OutputWindow = hwnd;
	desc.Windowed = TRUE;
	desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	desc.Flags = 0;


	D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&desc,
		&m_pSwapChain,
		&m_pDevice,
		nullptr,
		&m_pDeviceContext
	);

	ID3D11Resource* pBackBuffer = nullptr;
	m_pSwapChain->GetBuffer(0u, __uuidof(ID3D11Resource), reinterpret_cast<void**>(&pBackBuffer));

	if(pBackBuffer != nullptr)
		m_pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &m_pTargetView);
	pBackBuffer->Release();
}

Graphics::~Graphics() {
	if (m_pTargetView != nullptr)
		m_pTargetView->Release();
	if (m_pDeviceContext != nullptr)
		m_pDeviceContext->Release();
	if (m_pSwapChain != nullptr)
		m_pSwapChain->Release();
	if (m_pDevice != nullptr)
		m_pDevice->Release();
}

void Graphics::EndFrame() {
	m_pSwapChain->Present(1u, 0u);
}

void Graphics::ClearBuffer(float red, float green, float blue) noexcept {
	const float color[] = { red, green, blue, 1.0f };
	m_pDeviceContext->ClearRenderTargetView(m_pTargetView, color);
}