#include <Graphics.hpp>
#include <sstream>

#ifdef _DEBUG
#define GFX_THROW(hr) throw Graphics::HrException(__LINE__, __FILE__, hr, m_infoManager.GetMessages())
#define GFX_THROW_FAILED(hr, hrCall) m_infoManager.Set(); if(FAILED(hr = (hrCall))) GFX_THROW(hr)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException(__LINE__, __FILE__, hr, m_infoManager.GetMessages())
#else
#define GFX_THROW(hr) throw Graphics::HrException(__LINE__, __FILE__, hr)
#define GFX_THROW_FAILED(hr, hrCall) if(FAILED(hr = (hrCall))) GFX_THROW(hr)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException(__LINE__, __FILE__, hr)
#endif

// Graphics
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

	ID3D11Resource* pBackBuffer = nullptr;
	GFX_THROW_FAILED(hr, m_pSwapChain->GetBuffer(
		0u,
		__uuidof(ID3D11Resource),
		reinterpret_cast<void**>(&pBackBuffer)
	));

	GFX_THROW_FAILED(hr, m_pDevice->CreateRenderTargetView(
		pBackBuffer, nullptr, &m_pTargetView
	));
	pBackBuffer->Release();
}

Graphics::~Graphics() {
	if (m_pTargetView)
		m_pTargetView->Release();
	if (m_pDeviceContext)
		m_pDeviceContext->Release();
	if (m_pSwapChain)
		m_pSwapChain->Release();
	if (m_pDevice)
		m_pDevice->Release();
}

void Graphics::EndFrame() {
	HRESULT hr;

#ifdef _DEBUG
	m_infoManager.Set();
#endif
	if (FAILED(hr = m_pSwapChain->Present(1u, 0u))) {
		if (hr == DXGI_ERROR_DEVICE_REMOVED)
			throw GFX_DEVICE_REMOVED_EXCEPT(m_pDevice->GetDeviceRemovedReason());
		else
			GFX_THROW(hr);
	}
}

void Graphics::ClearBuffer(float red, float green, float blue) noexcept {
	const float color[] = { red, green, blue, 1.0f };
	m_pDeviceContext->ClearRenderTargetView(m_pTargetView, color);
}

// Graphics Exception
Graphics::HrException::HrException(int line, const char* file, HRESULT hr) noexcept
	: Xception(line, file), m_hr(hr) {}


Graphics::HrException::HrException(int line, const char* file, HRESULT hr,
	const std::vector<std::string>& infoMsgs) noexcept
	: Xception(line, file), m_hr(hr) {
	for (const std::string& m : infoMsgs) {
		m_info += m;
		m_info.append("\n");
	}

	if (!m_info.empty())
		m_info.pop_back();
}

const char* Graphics::HrException::what() const noexcept {
	std::ostringstream oss;
	oss << GetType() << "\n"
		<< "[Error code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << static_cast<std::uint64_t>(GetErrorCode()) << ")\n\n"
		<< "[Error String] " << GetErrorString() << "\n";
	if (!m_info.empty())
		oss << "[Error Info]\n" << GetErrorInfo() << "\n\n";
	oss << GetOriginString();
	m_whatBuffer = oss.str();

	return m_whatBuffer.c_str();
}

const char* Graphics::HrException::GetType() const noexcept {
	return "Graphics Exception";
}

const char* Graphics::DeviceRemovedException::GetType() const noexcept {
	return "Graphics Exception [Device Removed]";
}

HRESULT Graphics::HrException::GetErrorCode() const noexcept {
	return m_hr;
}

std::string Graphics::HrException::TranslateErrorCode(HRESULT hr) noexcept {
	char* pMsgBuf = nullptr;
	DWORD nMsgLen = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, static_cast<DWORD>(hr), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPSTR>(&pMsgBuf), 0, nullptr
	);

	if (!nMsgLen)
		return "Unidentified error code";

	std::string errorString = pMsgBuf;
	LocalFree(pMsgBuf);
	return errorString;
}

std::string Graphics::HrException::GetErrorString() const noexcept {
	return TranslateErrorCode(m_hr);
}

std::string Graphics::HrException::GetErrorInfo() const noexcept {
	return m_info;
}
