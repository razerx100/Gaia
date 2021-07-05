#ifndef __DRAWABLE_HPP__
#define __DRAWABLE_HPP__
#include <Graphics.hpp>
#include <Transform.hpp>
#include <memory>

class Bindable;

class Drawable {
public:
	Drawable() = default;
	virtual ~Drawable();

	virtual void Draw(Graphics& gfx) const noexcept(!IS_DEBUG);

	// Bind Root Signature before constant buffers
	void AddBind(std::unique_ptr<Bindable> bind) noexcept;
	void AddIndexBuffer(std::unique_ptr<class IndexBuffer> indexBuffer) noexcept;

protected:
	virtual std::uint32_t GetIndexCount() const noexcept;

protected:
	std::uint32_t m_IndexCount;
	std::vector<std::unique_ptr<Bindable>> m_Binds;

	Transform m_Transform;
};
#endif
