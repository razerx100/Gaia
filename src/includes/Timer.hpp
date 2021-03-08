#ifndef __TIMER_HPP__
#define __TIMER_HPP__
#include <chrono>

class Timer {
public:
	Timer() noexcept;
	float Mark() noexcept;
	float Peek() const noexcept;

private:
	std::chrono::steady_clock::time_point m_last;

};
#endif