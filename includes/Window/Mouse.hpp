#ifndef __MOUSE_HPP__
#define __MOUSE_HPP__
#include <queue>
#include <optional>

class Mouse {
	friend class Window;
public:
	struct RawDelta {
		int x;
		int y;
	};

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
		bool m_leftPressed;
		bool m_rightPressed;
		bool m_middlePressed;
		int m_x;
		int m_y;

	public:
		Event() noexcept
			: m_type(Type::Invalid),
			m_leftPressed(false),
			m_rightPressed(false),
			m_middlePressed(false),
			m_x(0),
			m_y(0) {}

		Event(Type type, const Mouse& parent) noexcept
			: m_type(type),
			m_leftPressed(parent.m_leftPressed),
			m_rightPressed(parent.m_rightPressed),
			m_middlePressed(parent.m_middlePressed),
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
			return m_leftPressed;
		}

		bool IsMiddlePressed() const noexcept {
			return m_middlePressed;
		}

		bool IsRightPressed() const noexcept {
			return m_rightPressed;
		}
	};

public:
	Mouse();

	Mouse(const Mouse&) = delete;
	Mouse& operator=(const Mouse&) = delete;

	std::pair<int, int> GetPos() const noexcept;
	int GetPosX() const noexcept;
	int GetPosY() const noexcept;
	std::optional<Event> Read() noexcept;
	std::optional<RawDelta> ReadRawDelta() noexcept;

	bool IsInWindow() const noexcept;
	bool IsLeftPressed() const noexcept;
	bool IsMiddlePressed() const noexcept;
	bool IsRightPressed() const noexcept;
	bool IsBufferEmpty() const noexcept;
	bool IsRawEnabled() const noexcept;

	void Flush() noexcept;
	void EnableRaw() noexcept;
	void DisableRaw() noexcept;

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
	void OnMouseRawDelta(int dx, int dy) noexcept;
	void TrimBuffer() noexcept;
	void TrimRawDeltaBuffer() noexcept;
	void OnWheelDelta(int delta) noexcept;

private:
	static constexpr unsigned int s_bufferSize = 16u;
	int m_x;
	int m_y;
	bool m_inWindow;
	bool m_leftPressed;
	bool m_middlePressed;
	bool m_rightPressed;
	int m_wheelDeltaCarry;
	bool m_rawEnabled;

	std::queue<Event> m_buffer;
	std::queue<RawDelta> m_rawDeltaBuffer;
};

#endif