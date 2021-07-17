#ifndef __UTILITIES_HPP__
#define __UTILITIES_HPP__
#include <string>

namespace GUtil {
	std::string GetNameFromPath(const std::string& path) noexcept;
	std::string GetFileRootFromPath(const std::string& path) noexcept;
}
#endif
