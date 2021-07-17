#include <Utilities.hpp>

namespace GUtil {
	std::string GetNameFromPath(const std::string& path) noexcept {
		std::uint64_t nameStart = path.find_last_of('\\');

		if (nameStart != std::string::npos)
			nameStart++;
		else
			nameStart = 0;

		return path.substr(nameStart, path.size() - nameStart);
	}

	std::string GetFileRootFromPath(const std::string& path) noexcept {
		std::uint64_t nameEndIndex = path.find_last_of('\\');

		return (nameEndIndex != std::string::npos ?
			path.substr(0u, nameEndIndex + 1) : "\\");
	}
}
