#ifndef __DRAWABLE_HPP__
#define __DRAWABLE_HPP__
#include <Graphics.hpp>
#include <Transform.hpp>
#include <memory>

class Bindable;

class Drawable {
public:
	Drawable() = default;
	virtual ~Drawable() = default;

	virtual void Update(float deltaTime) noexcept = 0;

	void Draw(Graphics& gfx) const noexcept(!IS_DEBUG);
	void AddBind(std::unique_ptr<Bindable> bind) noexcept;

	void AddIndexBuffer(std::unique_ptr<class IndexBuffer> indexBuffer) noexcept;

	static void SetShaderPath() noexcept;

	static std::wstring GetShaderPath() noexcept;

protected:
	virtual const std::vector<std::unique_ptr<Bindable>>& GetStaticBindables() const noexcept = 0;

	virtual std::uint32_t GetIndexCount() const noexcept = 0;

protected:
	static std::wstring s_ShaderPath;

	std::uint32_t m_IndexCount;
	std::vector<std::unique_ptr<Bindable>> m_Binds;

	Transform m_Transform;
};
#endif
