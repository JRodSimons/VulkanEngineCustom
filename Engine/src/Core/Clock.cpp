#include "PCH_Engine.h"

#include "Clock.h"
#include "Platform/Platform.h"

void Clock::Start()
{
	m_isActive = true;

	m_startTime = IPlatform_Base::Platform_GetAbsoluteTime();
}

void Clock::Stop()
{
	m_isActive = false;
}

void Clock::Update()
{
	if (m_isActive)
	{
		double currentTime = IPlatform_Base::Platform_GetAbsoluteTime();
		m_elapsedTime = currentTime - m_startTime;
	}
}