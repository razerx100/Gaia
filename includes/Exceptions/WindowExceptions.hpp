#ifndef __WINDOW_EXCEPTION_HPP__
#define __WINDOW_EXCEPTION_HPP__
#include <Exception.hpp>
typedef long HRESULT;

class WindowException : public Exception {
public:
	WindowException(int line, const char* file, HRESULT hr) noexcept;
	const char* what() const noexcept override;
	const char* GetType() const noexcept override;
	static std::string TranslateErrorCode(HRESULT hr) noexcept;
	HRESULT GetErrorCode() const noexcept;
	std::string GetErrorString() const noexcept;

private:
	HRESULT m_hr;
};

class NoGfxException : public Exception {
public:
	using Exception::Exception;
	const char* GetType() const noexcept override;
};

#endif