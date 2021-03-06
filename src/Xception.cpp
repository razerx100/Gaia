#include <Xception.hpp>
#include <sstream>

Xception::Xception(int line, const char* file) noexcept
	: m_line(line), m_file(file) {}

const char* Xception::what() const noexcept {
	std::ostringstream oss;
	oss << GetType() << "\n"
		<< GetOriginString();
	m_whatBuffer = oss.str();
	return m_whatBuffer.c_str();
}

const char* Xception::GetType() const noexcept {
	return "Exception";
}

int Xception::GetLine() const noexcept {
	return m_line;
}

const std::string& Xception::GetFile() const noexcept {
	return m_file;
}

std::string Xception::GetOriginString() const noexcept {
	std::ostringstream oss;
	oss << "[File] " << m_file << "\n"
		<< "[Line] " << m_line;

	return oss.str();
}