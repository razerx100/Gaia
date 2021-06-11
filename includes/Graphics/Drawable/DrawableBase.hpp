#ifndef __DRAWABLE_BASE_HPP__
#define __DRAWABLE_BASE_HPP__
#include <Drawable.hpp>
#include <Bindable.hpp>
#include <IndexBuffer.hpp>

template<typename T>
class DrawableBase : public Drawable {
public:
	void Draw(Graphics& gfx) const noexcept(!IS_DEBUG) override {
		for (auto& bind : s_Binds)
			bind->BindCommand(gfx);

		for (auto& bind : m_Binds)
			bind->BindCommand(gfx);

		gfx.DrawIndexed(GetIndexCount());
	}

	static bool IsDataInitialized() noexcept {
		return !s_Binds.empty();
	}

	// Bind Root Signature before constant buffers
	static void AddStaticBind(std::unique_ptr<Bindable> bind) noexcept {
		s_Binds.emplace_back(std::move(bind));
	}

	static void AddStaticIndexBuffer(std::unique_ptr<IndexBuffer> indexBuffer) noexcept {
		s_IndexCount = indexBuffer->GetIndexCount();
		s_Binds.emplace_back(std::move(indexBuffer));
	}

protected:
	// Override with m_IndexCount if drawable has different IndexBuffer for each object
	std::uint32_t GetIndexCount() const noexcept override {
		return s_IndexCount;
	}

private:
	static std::uint32_t s_IndexCount;
	static std::vector<std::unique_ptr<Bindable>> s_Binds;
};

template<typename T>
std::uint32_t DrawableBase<T>::s_IndexCount = 0;
template<typename T>
std::vector<std::unique_ptr<Bindable>> DrawableBase<T>::s_Binds;

#endif
