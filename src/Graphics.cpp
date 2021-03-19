#include <Graphics.hpp>
#include <sstream>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#ifdef _DEBUG
#define GFX_THROW_NO_HR(funCall) m_infoManager.Set(); funCall; std::vector<std::string> vec = m_infoManager.GetMessages(); if(!vec.empty()) throw Graphics::InfoException(__LINE__, __FILE__, vec)
#define GFX_THROW(hr) throw Graphics::HrException(__LINE__, __FILE__, hr, m_infoManager.GetMessages())
#define GFX_THROW_FAILED(hr, hrCall) m_infoManager.Set(); if(FAILED(hr = (hrCall))) GFX_THROW(hr)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException(__LINE__, __FILE__, hr, m_infoManager.GetMessages())
#else
#define GFX_THROW_NO_HR(funCall) funCall
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

	GetFullProjectPath();

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

	ComPtr<ID3D11Resource> pBackBuffer = nullptr;
	GFX_THROW_FAILED(hr, m_pSwapChain->GetBuffer(
		0u,
		__uuidof(ID3D11Resource),
		&pBackBuffer
	));

	GFX_THROW_FAILED(hr, m_pDevice->CreateRenderTargetView(
		pBackBuffer.Get(), nullptr, &m_pTargetView
	));
}

void Graphics::EndFrame() {
	HRESULT hr;

#ifdef _DEBUG
	m_infoManager.Set();
#endif
	if (FAILED(hr = m_pSwapChain->Present(0u, 0u))) {
		if (hr == DXGI_ERROR_DEVICE_REMOVED)
			throw GFX_DEVICE_REMOVED_EXCEPT(m_pDevice->GetDeviceRemovedReason());
		else
			GFX_THROW(hr);
	}
}

void Graphics::ClearBuffer(float red, float green, float blue) noexcept {
	const float color[] = { red, green, blue, 1.0f };
	m_pDeviceContext->ClearRenderTargetView(m_pTargetView.Get(), color);
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

	// Shader Path
	std::wstringstream shaderPath;
	shaderPath << m_ProjectPath.c_str() <<L"shaders\\precompiled\\" << BuildType() << L"\\";

	// Vertex Shader
	ComPtr<ID3D11VertexShader> pVertexShader;
	ComPtr<ID3DBlob> pBlob;
	GFX_THROW_FAILED(hr, D3DReadFileToBlob((shaderPath.str() + L"VertexShader.cso").c_str(), &pBlob));
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

	// Configure viewport
	D3D11_VIEWPORT vp = {};
	vp.Width = 1280.0f;
	vp.Height = 720.0f;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;

	m_pDeviceContext->RSSetViewports(1u, &vp);

	// Pixel Shader
	ComPtr<ID3D11PixelShader> pPixelShader;
	GFX_THROW_FAILED(hr, D3DReadFileToBlob((shaderPath.str() + L"PixelShader.cso").c_str(), &pBlob));
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

	m_pDeviceContext->OMSetRenderTargets(1u, m_pTargetView.GetAddressOf(), nullptr);

	GFX_THROW_NO_HR(m_pDeviceContext->DrawIndexed(static_cast<std::uint32_t>(std::size(indices)), 0u, 0u));
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

Graphics::InfoException::InfoException(int line, const char* file,
	const std::vector<std::string>& infoMsgs) noexcept
	: Xception(line, file) {

	for (const std::string msg : infoMsgs) {
		m_info += msg;
		m_info.append("\n");
	}

	if (!m_info.empty())
		m_info.pop_back();
}

const char* Graphics::InfoException::what() const noexcept {
	std::ostringstream oss;
	oss << GetType() << "\n\n";
	if (!m_info.empty())
		oss << "[Error Info]\n" << GetErrorInfo() << "\n\n";
	oss << GetOriginString();
	m_whatBuffer = oss.str();

	return m_whatBuffer.c_str();
}

const char* Graphics::InfoException::GetType() const noexcept {
	return "Graphics Info Exception";
}

std::string Graphics::InfoException::GetErrorInfo() const noexcept {
	return m_info;
}

// Utility
void Graphics::GetFullProjectPath() noexcept {
	m_ProjectPath = __FILE__;
	for (int i = static_cast<int>(m_ProjectPath.length()) - 1; m_ProjectPath[i] != '\\'; i--)
		m_ProjectPath.pop_back();
}
#ifdef _DEBUG
const char* Graphics::BuildType() const noexcept {
	return "Debug";
}
#elif NDEBUG
const char* Graphics::BuildType() const noexcept {
	return "Release";
}
#endif
