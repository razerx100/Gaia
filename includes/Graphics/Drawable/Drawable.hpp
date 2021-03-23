#ifndef __DRAWABLE_HPP__
#define __DRAWABLE_HPP__
#include <Graphics.hpp>
#include <DirectXMath.h>
#include <memory>
#include <BindableAll.hpp>

class Drawable {
public:
	Drawable() noexcept;
	virtual ~Drawable() = default;

	virtual void Update(Graphics& gfx, float deltaTime) noexcept = 0;

	void Draw(Graphics& gfx) const noexcept(!IS_DEBUG);
	void AddBind(std::unique_ptr<Bindable> bind) noexcept;
	void AddIndexBuffer(std::unique_ptr<IndexBuffer> indexBuffer) noexcept;
	void AddCVertexBuffer(std::unique_ptr<Mutable> vCBuffer) noexcept;

private:
	void SetShaderPath() noexcept;

protected:
	Mutable* m_VertexCBuffer;
	std::wstring m_ShaderPath;

private:
	std::uint32_t m_IndexCount;
	std::vector<std::unique_ptr<Bindable>> m_Binds;

};
#endif
