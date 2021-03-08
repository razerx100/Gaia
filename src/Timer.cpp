#include <Timer.hpp>

using namespace std::chrono;

Timer::Timer() noexcept {
	m_last = steady_clock::now();
}

float Timer::Mark() noexcept {
	const steady_clock::time_point old = m_last;
	m_last = steady_clock::now();
	const duration<float> frameTime = m_last - old;
	return frameTime.count();
}

float Timer::Peek() const noexcept {
	return duration<float>(steady_clock::now() - m_last).count();
}