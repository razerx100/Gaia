#include <Utilities.hpp>
#include <CleanWin.hpp>

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

	std::string ShaderPath::s_shaderPath;

	void ShaderPath::Set() {
		char path[MAX_PATH];
		GetModuleFileNameA(nullptr, path, MAX_PATH);
		s_shaderPath = path;
		for (int i = static_cast<int>(s_shaderPath.size() - 1); s_shaderPath[i] != '\\'; i--)
			s_shaderPath.pop_back();

		s_shaderPath.append("shaders\\");
	}

	std::string ShaderPath::Get() noexcept {
		return s_shaderPath;
	}
}
