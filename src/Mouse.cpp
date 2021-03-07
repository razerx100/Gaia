#include <Mouse.hpp>

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
	return m_InWindow;
}

bool Mouse::IsLeftPressed() const noexcept {
	return m_LeftPressed;
}

bool Mouse::IsMiddlePressed() const noexcept {
	return m_MiddlePressed;
}

bool Mouse::IsRightPressed() const noexcept {
	return m_RightPressed;
}

Mouse::Event Mouse::Read() noexcept {
	if (!m_buffer.empty()) {
		Mouse::Event e = m_buffer.front();
		m_buffer.pop();
		return e;
	}
	else
		return Mouse::Event();
}

bool Mouse::IsEmpty() const noexcept {
	return m_buffer.empty();
}

void Mouse::Flush() noexcept {
	m_buffer = std::queue<Event>();
}

void Mouse::OnMouseMove(int x, int y) noexcept {
	m_x = x;
	m_y = y;

	m_buffer.emplace(Mouse::Event(Mouse::Event::Type::Move, *this));
	TrimBuffer();
}

void Mouse::OnMouseEnter() noexcept {
	m_InWindow = true;

	m_buffer.emplace(Mouse::Event(Mouse::Event::Type::Enter, *this));
	TrimBuffer();
}

void Mouse::OnMouseLeave() noexcept {
	m_InWindow = false;

	m_buffer.emplace(Mouse::Event(Mouse::Event::Type::Leave, *this));
	TrimBuffer();
}

void Mouse::OnLeftPress() noexcept {
	m_LeftPressed = true;

	m_buffer.emplace(Mouse::Event(Mouse::Event::Type::LPress, *this));
	TrimBuffer();
}

void Mouse::OnMiddlePress() noexcept {
	m_MiddlePressed = true;

	m_buffer.emplace(Mouse::Event(Mouse::Event::Type::MPress, *this));
	TrimBuffer();
}

void Mouse::OnRightPress() noexcept {
	m_RightPressed = true;

	m_buffer.emplace(Mouse::Event(Mouse::Event::Type::RPress, *this));
	TrimBuffer();
}

void Mouse::OnLeftRelease() noexcept {
	m_LeftPressed = false;

	m_buffer.emplace(Mouse::Event(Mouse::Event::Type::LRelease, *this));
	TrimBuffer();
}

void Mouse::OnMiddleRelease() noexcept {
	m_MiddlePressed = false;

	m_buffer.emplace(Mouse::Event(Mouse::Event::Type::MRelease, *this));
	TrimBuffer();
}

void Mouse::OnRightRelease() noexcept {
	m_RightPressed = false;

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
