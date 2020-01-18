#pragma once
#include <chrono>

class Timer
{
public:
	Timer();
	void Reset();
	float Elapsed() const;
private:
	std::chrono::steady_clock::time_point m_Start;
};
