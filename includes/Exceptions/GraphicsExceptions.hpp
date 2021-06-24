#ifndef __GRAPHICS_EXCEPTIONS_HPP__
#define __GRAPHICS_EXCEPTIONS_HPP__
#include <Exception.hpp>
#include <vector>

class HrException : public Exception {
public:
	HrException(int line, const char* file, long hr) noexcept;
	HrException(
		int line, const char* file,
		long hr, const std::vector<std::string>& infoMsgs
	) noexcept;
	const char* what() const noexcept override;
	const char* GetType() const noexcept override;
	long GetErrorCode() const noexcept;
	static std::string TranslateErrorCode(long hr) noexcept;
	std::string GetErrorString() const noexcept;
	std::string GetErrorInfo() const noexcept;

private:
	long m_hr;
	std::string m_info;
};

class DeviceRemovedException : public HrException {
public:
	using HrException::HrException;
	const char* GetType() const noexcept override;
};

class InfoException : public Exception {
public:
	InfoException(int line, const char* file,
		const std::vector<std::string>& infoMsgs
	) noexcept;
	const char* what() const noexcept override;
	const char* GetType() const noexcept override;
	std::string GetErrorInfo() const noexcept;

private:
	std::string m_info;
};

class D3D12NotSupportedException : public std::exception {
public:
	const char* what() const noexcept override;
};

#endif