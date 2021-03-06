#ifndef __XCEPTION_HPP__
#define __XCEPTION_HPP__
#include <exception>
#include <string>

class Xception : public std::exception {
public:
	Xception(int line, const char* file) noexcept;
	const char* what() const noexcept override;
	virtual const char* GetType() const noexcept;
	int GetLine() const noexcept;
	const std::string& GetFile() const noexcept;
	std::string GetOriginString() const noexcept;

private:
	int m_line;
	std::string m_file;

protected:
	mutable std::string m_whatBuffer;
};

#endif