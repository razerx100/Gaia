#include<Keyboard.hpp>

bool Keyboard::IsKeyPressed(unsigned char keycode) const noexcept {
	return m_keystates[keycode];
}

Keyboard::Event Keyboard::ReadKey() noexcept {
	if (m_keyBuffer.size() > 0u) {
		Keyboard::Event e = m_keyBuffer.front();
		m_keyBuffer.pop();
		return e;
	}
	else
		return Keyboard::Event();
}

bool Keyboard::IsKeyEmpty() const noexcept {
	return m_keyBuffer.empty();
}

char Keyboard::ReadChar() noexcept {
	if (m_charBuffer.size() > 0u) {
		unsigned char charCode = m_charBuffer.front();
		m_charBuffer.pop();
		return charCode;
	}
	else
		return 0;
}

bool Keyboard::IsCharEmpty() const noexcept {
	return m_charBuffer.empty();
}

void Keyboard::FlushChar() noexcept {
	m_charBuffer = std::queue<char>();
}

void Keyboard::FlushKey() noexcept {
	m_keyBuffer = std::queue<Event>();
}

void Keyboard::Flush() noexcept {
	FlushKey();
	FlushChar();
}

void Keyboard::EnableAutoRepeat() noexcept {
	m_autoRepeatEnabled = true;
}

void Keyboard::DisableAutoRepeat() noexcept {
	m_autoRepeatEnabled = false;
}

bool Keyboard::IsAutoRepeatEnabled() const noexcept {
	return m_autoRepeatEnabled;
}

void Keyboard::OnKeyPressed(unsigned char keycode) noexcept {
	m_keystates[keycode] = true;
	m_keyBuffer.push(Keyboard::Event(Keyboard::Event::Type::Press, keycode));
	TrimBuffer(m_keyBuffer);
}

void Keyboard::OnKeyReleased(unsigned char keycode) noexcept {
	m_keystates[keycode] = false;
	m_keyBuffer.push(Keyboard::Event(Keyboard::Event::Type::Release, keycode));
	TrimBuffer(m_keyBuffer);
}

void Keyboard::OnChar(char character) noexcept {
	m_charBuffer.push(character);
	TrimBuffer(m_charBuffer);
}

void Keyboard::ClearState() noexcept {
	m_keystates.reset();
}

template<typename T>
void Keyboard::TrimBuffer(std::queue<T>& buffer) noexcept {
	while (buffer.size() > s_bufferSize)
		buffer.pop();
}
