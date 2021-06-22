#include <ResourceView.hpp>
#include <GraphicsThrowMacros.hpp>

ResourceView::ResourceView(std::uint32_t descriptorIndex)
	: m_descriptorIndex(descriptorIndex) {}

std::uint32_t ResourceView::GetDescriptorIndex() const noexcept {
	return m_descriptorIndex;
}
