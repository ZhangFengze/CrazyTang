#include "Timer.h"

Timer::Timer()
{
	Reset();
}

void Timer::Reset()
{
	m_Start = std::chrono::steady_clock::now();
}

float Timer::Elapsed() const
{
	auto now = std::chrono::steady_clock::now();
	auto elapsed = now - m_Start;
	return std::chrono::duration_cast<
		std::chrono::duration<float, std::ratio<1, 1>>>(elapsed).count();
}
