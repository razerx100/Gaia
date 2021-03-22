#ifndef __KEYBOARD_HPP__
#define __KEYBOARD_HPP__
#include <queue>
#include <bitset>

class Keyboard {
	friend class Window;

public:
	class Event {
	public:
		enum class Type {
			Press,
			Release,
			Invalid
		};

	private:
		Type m_type;
		unsigned char m_code;

	public:
		Event() noexcept
			: m_type(Type::Invalid), m_code(0u) {}
		Event(Type type, unsigned char code) noexcept
			: m_type(type), m_code(code) {}
		bool IsPress() const noexcept {
			return m_type == Type::Press;
		}
		bool IsRelease() const noexcept {
			return m_type == Type::Release;
		}
		bool IsValid() const noexcept {
			return m_type != Type::Invalid;
		}
		unsigned char GetCode() const noexcept {
			return m_code;
		}
	};

public:
	Keyboard()
		: m_autoRepeatEnabled(false) {}
	Keyboard(const Keyboard&) = delete;
	Keyboard& operator=(const Keyboard&) = delete;

	// key events
	bool IsKeyPressed(unsigned char keycode) const noexcept;
	Event ReadKey() noexcept;
	bool IsKeyEmpty() const noexcept;
	void FlushKey() noexcept;

	// char events
	char ReadChar() noexcept;
	bool IsCharEmpty() const noexcept;
	void FlushChar() noexcept;
	void Flush() noexcept;

	// auto-repeat control
	void EnableAutoRepeat() noexcept;
	void DisableAutoRepeat() noexcept;
	bool IsAutoRepeatEnabled() const noexcept;

private:
	void OnKeyPressed(unsigned char keycode) noexcept;
	void OnKeyReleased(unsigned char keycode) noexcept;
	void OnChar(char character) noexcept;
	void ClearState() noexcept;
	template<typename T>
	static void TrimBuffer(std::queue<T>& buffer) noexcept;

private:
	static constexpr unsigned int s_nKeys = 256u;
	static constexpr unsigned int s_bufferSize = 16u;
	bool m_autoRepeatEnabled;
	std::bitset<s_nKeys> m_keystates;
	std::queue<Event> m_keyBuffer;
	std::queue<char> m_charBuffer;
};

#endif