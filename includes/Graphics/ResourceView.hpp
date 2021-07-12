#ifndef __RESOURCE_VIEW_HPP__
#define __RESOURCE_VIEW_HPP__
#include <cinttypes>
#include <string>

class ResourceView {
public:
	ResourceView(std::uint32_t descriptorIndex);
	virtual ~ResourceView() = default;

	std::uint32_t GetDescriptorIndex() const noexcept;

protected:
	std::uint32_t m_descriptorIndex;
	long hr;
};
#endif
