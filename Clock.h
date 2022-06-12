#pragma once
#include <chrono>

class Clock
{
public:
	Clock() { m_start = std::chrono::high_resolution_clock::now(); }

	~Clock(){
		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> duration = end - m_start;

		//std::cout << "\nDuration: " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << '\n';
	}
private:
	std::chrono::time_point<std::chrono::steady_clock> m_start;
};
