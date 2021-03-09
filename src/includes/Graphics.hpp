#ifndef __GRAPHICS_HPP__
#define __GRAPHICS_HPP__
#include <CleanWin.hpp>
#include <Xception.hpp>
#include <DxgiInfoManager.hpp>
#include <d3d11.h>

class Graphics {
public:
	class HrException : public Xception {
	public:
		HrException(int line, const char* file, HRESULT hr) noexcept;
		HrException(int line, const char* file, HRESULT hr, const std::vector<std::string>& infoMsgs) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		static std::string TranslateErrorCode(HRESULT hr) noexcept;
		std::string GetErrorString() const noexcept;
		std::string GetErrorInfo() const noexcept;

	private:
		HRESULT m_hr;
		std::string m_info;

	};

	class DeviceRemovedException : public HrException {
	public:
		using HrException::HrException;
		const char* GetType() const noexcept override;
	};

public:
	Graphics(HWND hwnd);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics();

	void EndFrame();
	void ClearBuffer(float red, float green, float blue) noexcept;

private:
#ifdef _DEBUG
	DxgiInfoManager m_infoManager;
#endif
	ID3D11Device* m_pDevice;
	IDXGISwapChain* m_pSwapChain;
	ID3D11DeviceContext* m_pDeviceContext;
	ID3D11RenderTargetView* m_pTargetView;

};
#endif