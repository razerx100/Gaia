#ifndef __GRAPHICS_EXCEPTIONS_HPP__
#define __GRAPHICS_EXCEPTIONS_HPP__
#include <Exception.hpp>
#include <vector>

typedef long HRESULT;

class HrException : public Exception {
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

class InfoException : public Exception {
public:
	InfoException(int line, const char* file, const std::vector<std::string>& infoMsgs) noexcept;
	const char* what() const noexcept override;
	const char* GetType() const noexcept override;
	std::string GetErrorInfo() const noexcept;

private:
	std::string m_info;
};

#endif