#pragma once

#include <iostream>
#include <chrono>
typedef std::chrono::high_resolution_clock Clock;

class ScopedTimer
{

public:

	ScopedTimer() : m_startTime(std::chrono::high_resolution_clock::now()) {}
	~ScopedTimer() {}
	
	void Reset() 
	{
		m_startTime = std::chrono::high_resolution_clock::now();
	};

	double ElapsedTimeInSeconds() const 
	{
		auto currentTime = std::chrono::high_resolution_clock::now();
		auto elaspedTime = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - m_startTime);
		return elaspedTime.count() * 1.0e-6; // Convert to seconds
	}

	double ElapsedTimeInMilliseconds() const
	{
		return ElapsedTimeInSeconds() * 1000;
	}

private:
	std::chrono::time_point<std::chrono::high_resolution_clock> m_startTime;
};