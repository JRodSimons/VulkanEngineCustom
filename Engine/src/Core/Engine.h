#pragma once

#include "EngineGlobals.h"
#include "Renderer/Renderer_Interface.h"

#include "Core/Clock.h"


class IGame;
class IPlatform_Base;
class Memory_System;
class Event_System;
class Input_System;
class TextureSystem;

class IRendererFrontend;
enum renderingAPIChoices;

// TODO: Temp
class VulkanRenderer;

class IEngine
{
public:
	ENGINE_API bool Engine_Initialize();

	ENGINE_API bool Engine_Run();

	ENGINE_API void Engine_Shutdown();

	IEngine* getEngineHandle() { return this; }

	double GetCurrentTime() { return m_engineClock.GetElapsedTime(); }

	// System Pointers
	// Memory system
	Memory_System* m_memorySystem;

	// Event system
	Event_System* m_eventSystem;

	// Input
	Input_System* m_inputSystem;

	// Platform layer
	IPlatform_Base* m_PlatformLayer;

	// Rendering Interface
	IRendererFrontend* m_RendererFrontend;

	// Texture System
	TextureSystem* m_textureSystem;

	// Game Layer
	IGame* m_gameLayer;

	const char* m_applicationName;
	int m_startXPos;
	int m_startYPos;
	unsigned int m_width;
	unsigned int m_height;
	bool		 m_isSuspended;

	unsigned int m_currentFrame;

	renderingAPIChoices m_RenderingAPI = API_UNDEFINED;

	bool m_bApplicationQuitCalled = false;

	// Engine clock
	inline static Clock m_engineClock;
	double m_lastTime;
	unsigned int m_FPS;
};
