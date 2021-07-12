#ifndef __BINDABLE_HPP__
#define __BINDABLE_HPP__
#include <CleanWin.hpp>
#include <GraphicsExtractor.hpp>
#include <wrl.h>
#include <string>
#include <cstdint>
#include <memory>

using Microsoft::WRL::ComPtr;

class Bindable : public GraphicsExtractor {
public:
	Bindable();
	virtual ~Bindable() = default;

	virtual void BindCommand(Graphics& gfx) noexcept = 0;

protected:
	HRESULT hr;
};

namespace GUtil {
	std::string GetNameFromPath(const std::string& path) noexcept;
	std::string GetFileRootFromPath(const std::string& path) noexcept;
}
#endif
