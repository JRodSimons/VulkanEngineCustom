#pragma once
#include "Engine.h"

#define MAX_EVENTS 256

enum Event_Type
{
	EVENT_TYPE_UNKNOWN = 0,
	EVENT_TYPE_WINDOW_RESIZE,
	EVENT_TYPE_APPLICATION_QUIT,
	EVENT_TYPE_INPUT_KEY_PRESSED,
	EVENT_TYPE_INPUT_KEY_RELEASED,
	EVENT_TYPE_INPUT_MOUSE_BPRESSED,
	EVENT_TYPE_INPUT_MOUSE_BRELEASED,
	EVENT_TYPE_INPUT_MOUSE_MOVED,
};

typedef bool (*Event_Callback) (void* data);

class Event
{
public:
	friend class Event_System;

private:
	Event_Callback m_PFNEventCallback;

	int m_EventID;
	Event_Type m_EventType;
};

class Event_System
{
public:
	bool EventSystem_Initialize(IEngine* enginePtr);
	void EventSystem_Shutdown(IEngine* enginePtr);

	bool ENGINE_API EventRegister(Event_Type type, Event_Callback PFN_Callback);
	bool ENGINE_API EventUnregister(Event_Type type, Event_Callback PFN_Callback);
	bool ENGINE_API EventFire(Event_Type type, void* data);

private:
	Event m_RegisteredEvents[MAX_EVENTS];
};