#include "PCH_Engine.h"
#include "Event.h"
#include "Memory.h"

using namespace std;

//static Event_System* event_system_ptr = 0;

bool Event_System::EventSystem_Initialize(IEngine* enginePtr)
{
	// Loop through events and invalidate entries.
	for (int i = 0; i < MAX_EVENTS; i++)
	{
		m_RegisteredEvents[i].m_EventType = EVENT_TYPE_UNKNOWN;
		m_RegisteredEvents[i].m_EventID = -1;
		m_RegisteredEvents[i].m_PFNEventCallback = 0;
	}

	LOG_INFO("Event System Initialized!");
	return true;
}

void Event_System::EventSystem_Shutdown(IEngine* enginePtr)
{
	LOG_VERBOSE("EventSystem_Shutdown called");

	for (int i = 0; i < MAX_EVENTS; i++)
	{
		if (m_RegisteredEvents[i].m_EventID != -1)
		{
			LOG_ERROR("Event_System shutting down with events still registered, clean this up...");
		}

		enginePtr->m_memorySystem->Mem_Free(this);
		enginePtr->m_eventSystem = 0;

		return;
	}
}

bool Event_System::EventRegister(Event_Type type, Event_Callback PFN_Callback)
{
	static int m_numRegisteredEvents = 0;
	if (!type || !PFN_Callback)
	{
		LOG_ERROR("EventSystem::EventRegister requires a valid Event_Type and Callback function pointer.");
		return false;
	}

	Event* newEvent = m_RegisteredEvents;

	// Iterate through the registered events until a free slot is found
	for (m_numRegisteredEvents; m_numRegisteredEvents < MAX_EVENTS; m_numRegisteredEvents++)
	{
		if (newEvent[m_numRegisteredEvents].m_EventID == -1)
		{
			// Found a free slot, populate
			newEvent[m_numRegisteredEvents].m_EventID = m_numRegisteredEvents + 1;
			newEvent[m_numRegisteredEvents].m_EventType = type;
			newEvent[m_numRegisteredEvents].m_PFNEventCallback = PFN_Callback;

			m_numRegisteredEvents++;

			return true;
		}
	}

	// If we make it here, then there are no more event slots available.
	LOG_ERROR("Cannot register new event, m_RegisteredEvents is full!");
	return false;
}

bool Event_System::EventUnregister(Event_Type type, Event_Callback PFN_Callback)
{
	if (!type || !PFN_Callback)
	{
		LOG_ERROR("EventSystem::EventUnregister requires a valid Event_Type and Callback function pointer.");
		return false;
	}

	Event* freeEvent = m_RegisteredEvents;

	for (int i = 0; i < MAX_EVENTS; i++)
	{
		if (freeEvent[i].m_EventType == type && freeEvent[i].m_PFNEventCallback == PFN_Callback)
		{
			// Found the corresponding event entry, unregister and uninitialize.
			freeEvent[i].m_EventType = EVENT_TYPE_UNKNOWN;
			freeEvent[i].m_PFNEventCallback = 0;
			freeEvent[i].m_EventID = -1;

			return true;
		}
		else
			continue;

		LOG_ERROR("Event System::EventUnregister - requested event can't be found. No action taken.");
		return false;
	}
}

bool Event_System::EventFire(Event_Type type, void* data)
{
	Event* Event = m_RegisteredEvents;

	// TODO: Loop only through events of the SAME type, we need to speed this loop up...

	for (int i = 0; i < MAX_EVENTS; i++)
	{
		if (Event[i].m_EventType == type)
		{
			Event[i].m_PFNEventCallback(data);
			continue;
		}
		else
			continue;
	}

	return true;
}