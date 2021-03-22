#include <CleanWin.hpp>
#include <GraphicsExceptions.hpp>
#include <sstream>

HrException::HrException(int line, const char* file, HRESULT hr) noexcept
	: Exception(line, file), m_hr(hr) {}


HrException::HrException(int line, const char* file, HRESULT hr,
	const std::vector<std::string>& infoMsgs) noexcept
	: Exception(line, file), m_hr(hr) {
	for (const std::string& m : infoMsgs) {
		m_info += m;
		m_info.append("\n");
	}

	if (!m_info.empty())
		m_info.pop_back();
}

const char* HrException::what() const noexcept {
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

const char* HrException::GetType() const noexcept {
	return "Graphics Exception";
}

const char* DeviceRemovedException::GetType() const noexcept {
	return "Graphics Exception [Device Removed]";
}

HRESULT HrException::GetErrorCode() const noexcept {
	return m_hr;
}

std::string HrException::TranslateErrorCode(HRESULT hr) noexcept {
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

std::string HrException::GetErrorString() const noexcept {
	return TranslateErrorCode(m_hr);
}

std::string HrException::GetErrorInfo() const noexcept {
	return m_info;
}

InfoException::InfoException(int line, const char* file,
	const std::vector<std::string>& infoMsgs) noexcept
	: Exception(line, file) {

	for (const std::string msg : infoMsgs) {
		m_info += msg;
		m_info.append("\n");
	}

	if (!m_info.empty())
		m_info.pop_back();
}

const char* InfoException::what() const noexcept {
	std::ostringstream oss;
	oss << GetType() << "\n\n";
	if (!m_info.empty())
		oss << "[Error Info]\n" << GetErrorInfo() << "\n\n";
	oss << GetOriginString();
	m_whatBuffer = oss.str();

	return m_whatBuffer.c_str();
}

const char* InfoException::GetType() const noexcept {
	return "Graphics Info Exception";
}

std::string InfoException::GetErrorInfo() const noexcept {
	return m_info;
}
