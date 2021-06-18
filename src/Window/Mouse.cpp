#include <Mouse.hpp>

Mouse::Mouse()
	: m_x(0),
	m_y(0),
	m_inWindow(false),
	m_leftPressed(false),
	m_middlePressed(false),
	m_rightPressed(false),
	m_wheelDeltaCarry(0),
	m_rawEnabled(false) {}

std::pair<int, int> Mouse::GetPos() const noexcept {
	return { m_x, m_y };
}

int Mouse::GetPosX() const noexcept {
	return m_x;
}

int Mouse::GetPosY() const noexcept {
	return m_y;
}

bool Mouse::IsInWindow() const noexcept {
	return m_inWindow;
}

bool Mouse::IsLeftPressed() const noexcept {
	return m_leftPressed;
}

bool Mouse::IsMiddlePressed() const noexcept {
	return m_middlePressed;
}

bool Mouse::IsRightPressed() const noexcept {
	return m_rightPressed;
}

bool Mouse::IsRawEnabled() const noexcept {
	return m_rawEnabled;
}

std::optional<Mouse::Event> Mouse::Read() noexcept {
	if (!m_buffer.empty()) {
		Mouse::Event e = m_buffer.front();
		m_buffer.pop();
		return e;
	}
	else
		return std::nullopt;
}

bool Mouse::IsBufferEmpty() const noexcept {
	return m_buffer.empty();
}

void Mouse::Flush() noexcept {
	m_buffer = std::queue<Event>();
}

void Mouse::EnableRaw() noexcept {
	m_rawEnabled = true;
}

void Mouse::DisableRaw() noexcept {
	m_rawEnabled = false;
}

void Mouse::OnMouseMove(int x, int y) noexcept {
	m_x = x;
	m_y = y;

	m_buffer.emplace(Mouse::Event(Mouse::Event::Type::Move, *this));
	TrimBuffer();
}

void Mouse::OnMouseEnter() noexcept {
	m_inWindow = true;

	m_buffer.emplace(Mouse::Event(Mouse::Event::Type::Enter, *this));
	TrimBuffer();
}

void Mouse::OnMouseLeave() noexcept {
	m_inWindow = false;

	m_buffer.emplace(Mouse::Event(Mouse::Event::Type::Leave, *this));
	TrimBuffer();
}

void Mouse::OnLeftPress() noexcept {
	m_leftPressed = true;

	m_buffer.emplace(Mouse::Event(Mouse::Event::Type::LPress, *this));
	TrimBuffer();
}

void Mouse::OnMiddlePress() noexcept {
	m_middlePressed = true;

	m_buffer.emplace(Mouse::Event(Mouse::Event::Type::MPress, *this));
	TrimBuffer();
}

void Mouse::OnRightPress() noexcept {
	m_rightPressed = true;

	m_buffer.emplace(Mouse::Event(Mouse::Event::Type::RPress, *this));
	TrimBuffer();
}

void Mouse::OnLeftRelease() noexcept {
	m_leftPressed = false;

	m_buffer.emplace(Mouse::Event(Mouse::Event::Type::LRelease, *this));
	TrimBuffer();
}

void Mouse::OnMiddleRelease() noexcept {
	m_middlePressed = false;

	m_buffer.emplace(Mouse::Event(Mouse::Event::Type::MRelease, *this));
	TrimBuffer();
}

void Mouse::OnRightRelease() noexcept {
	m_rightPressed = false;

	m_buffer.emplace(Mouse::Event(Mouse::Event::Type::RRelease, *this));
	TrimBuffer();
}

void Mouse::OnWheelUp() noexcept {
	m_buffer.emplace(Mouse::Event(Mouse::Event::Type::WheelUp, *this));
	TrimBuffer();
}

void Mouse::OnWheelDown() noexcept {
	m_buffer.emplace(Mouse::Event(Mouse::Event::Type::WheelDown, *this));
	TrimBuffer();
}

void Mouse::TrimBuffer() noexcept {
	while (m_buffer.size() > s_bufferSize)
		m_buffer.pop();
}

void Mouse::OnWheelDelta(int delta) noexcept {
	m_wheelDeltaCarry += delta;

	while (m_wheelDeltaCarry >= 120) {
		m_wheelDeltaCarry -= 120;
		OnWheelUp();
	}
	while (m_wheelDeltaCarry <= -120) {
		m_wheelDeltaCarry += 120;
		OnWheelDown();
	}
}

void Mouse::OnMouseRawDelta(int dx, int dy) noexcept {
	m_rawDeltaBuffer.emplace(RawDelta{dx, dy});
	TrimRawDeltaBuffer();
}

void Mouse::TrimRawDeltaBuffer() noexcept {
	while (m_rawDeltaBuffer.size() > s_bufferSize)
		m_rawDeltaBuffer.pop();
}

std::optional<Mouse::RawDelta> Mouse::ReadRawDelta() noexcept {
	if (m_rawDeltaBuffer.empty())
		return std::nullopt;

	RawDelta d = m_rawDeltaBuffer.front();
	m_rawDeltaBuffer.pop();

	return d;
}
