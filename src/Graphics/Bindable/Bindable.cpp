#include <Bindable.hpp>
#include <typeinfo>

Bindable::Bindable() : hr(0u) {}

namespace GUtil {
	std::string GetNameFromPath(const std::string& path) noexcept {
		std::uint64_t nameStart = 0u, nameEnd = 0u;
		for (nameStart = path.size() - 1u; path[nameStart] != '\\'; --nameStart);
		for (nameEnd = path.size() - 1u; path[nameEnd] != '.'; --nameEnd);
		nameStart++;

		return path.substr(nameStart, nameEnd - nameStart);
	}

	std::string GetFileRootFromPath(const std::string& path) noexcept {
		std::uint64_t index = 0u;
		for (index = path.size() - 1u; path[index] != '\\'; --index);

		return path.substr(0u, index + 1);
	}
}
