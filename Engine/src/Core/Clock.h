#pragma once

#include "Core/Logging.h"

class ENGINE_API Clock
{
public:
	Clock()
		: m_startTime(0),
		m_elapsedTime(0),
		  m_isActive(false)
	{
		
	}

	void Start();

	void Stop();

	void Update();

	double GetElapsedTime()
	{
		if (!m_isActive)
		{
			LOG_ERROR("Clock - GetElapsedTime not initialized");
		}
		else
		{
			return m_elapsedTime;
		}
	}

	double GetStartTime() { return m_startTime; }

	bool IsClockActive() { return m_isActive; }

private:
	double m_startTime;
	double m_elapsedTime;

	bool m_isActive;
};