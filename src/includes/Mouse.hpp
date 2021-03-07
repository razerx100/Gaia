#ifndef __MOUSE_HPP__
#define __MOUSE_HPP__
#include <queue>

class Mouse {
	friend class Window;
public:
	class Event {
	public:
		enum class Type {
			LPress,
			LRelease,
			MPress,
			MRelease,
			RPress,
			RRelease,
			WheelUp,
			WheelDown,
			Move,
			Enter,
			Leave,
			Invalid
		};

	private:
		Type m_type;
		bool m_LeftPressed;
		bool m_RightPressed;
		bool m_MiddlePressed;
		int m_x;
		int m_y;

	public:
		Event() noexcept
			: m_type(Type::Invalid),
			m_LeftPressed(false),
			m_RightPressed(false),
			m_MiddlePressed(false),
			m_x(0),
			m_y(0) {}

		Event(Type type, const Mouse& parent) noexcept
			: m_type(type),
			m_LeftPressed(parent.m_LeftPressed),
			m_RightPressed(parent.m_RightPressed),
			m_MiddlePressed(parent.m_MiddlePressed),
			m_x(parent.m_x),
			m_y(parent.m_y) {}

		bool IsValid() const noexcept {
			return m_type != Type::Invalid;
		}

		Type GetType() const noexcept {
			return m_type;
		}

		std::pair<int, int> GetPos() const noexcept {
			return { m_x, m_y };
		}

		int GetPosX() const noexcept {
			return m_x;
		}

		int GetPosY() const noexcept {
			return m_y;
		}

		bool IsLeftPressed() const noexcept {
			return m_LeftPressed;
		}

		bool IsMiddlePressed() const noexcept {
			return m_MiddlePressed;
		}

		bool IsRightPressed() const noexcept {
			return m_RightPressed;
		}
	};

public:
	Mouse()
		: m_x(0),
		m_y(0),
		m_InWindow(false),
		m_LeftPressed(false),
		m_MiddlePressed(false),
		m_RightPressed(false),
		m_wheelDeltaCarry(0) {}

	Mouse(const Mouse&) = delete;
	Mouse& operator=(const Mouse&) = delete;

	std::pair<int, int> GetPos() const noexcept;
	int GetPosX() const noexcept;
	int GetPosY() const noexcept;
	bool IsInWindow() const noexcept;
	bool IsLeftPressed() const noexcept;
	bool IsMiddlePressed() const noexcept;
	bool IsRightPressed() const noexcept;
	Mouse::Event Read() noexcept;
	bool IsEmpty() const noexcept;
	void Flush() noexcept;

private:
	void OnMouseMove(int x, int y) noexcept;
	void OnMouseLeave() noexcept;
	void OnMouseEnter() noexcept;
	void OnLeftPress() noexcept;
	void OnMiddlePress() noexcept;
	void OnRightPress() noexcept;
	void OnLeftRelease() noexcept;
	void OnMiddleRelease() noexcept;
	void OnRightRelease() noexcept;
	void OnWheelUp() noexcept;
	void OnWheelDown() noexcept;
	void TrimBuffer() noexcept;
	void OnWheelDelta(int delta) noexcept;

private:
	static constexpr unsigned int s_bufferSize = 16u;
	int m_x;
	int m_y;
	bool m_InWindow;
	bool m_LeftPressed;
	bool m_MiddlePressed;
	bool m_RightPressed;
	int m_wheelDeltaCarry;
	std::queue<Event> m_buffer;
};

#endif