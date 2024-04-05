#include "PCH_Engine.h"

#include "Engine.h"
#include "GameClass.h"

// TODO: temp
#include "Platform/Platform.h"
#include "Core/Memory.h"
#include "Core/Logging.h"
#include "Core/Event.h"
#include "Core/Input.h"
#include "Core/String.h"
#include "Core/Clock.h"
#include "Renderer/Renderer_Interface.h"
#include "Systems/TextureSystem.h"
#include "Math/Math.h"
#include "Math/Math_Types.h"
#include "Resources/Filesystem.h"
#include "Resources/Shader.h"

// Internal Engine Pointer to track the engine interface.
static IEngine* g_enginePtr = 0;

bool PFN_Application_Quit(void* data);
bool PFN_On_Key_Pressed(void* data);
bool PFN_On_Key_Released(void* data);

bool IEngine::Engine_Initialize()
{
	if (g_enginePtr != NULL)
	{
		LOG_FATAL("Engine_Initialize() cannot be called more than once!");
		return false;
	}

	g_enginePtr = this;

	m_applicationName = "Engine Testbed";
	m_width = 1080;
	m_height = 720;

	m_currentFrame = 0;
	m_isSuspended = false;

	m_RenderingAPI = API_VULKAN;

	// Initialize subsystems
	// TODO: Move to system manager

	// Memory
	// TODO: DONT USE NEW HERE!
	m_memorySystem = new Memory_System;

	if (!m_memorySystem->MemorySystem_Initialize(g_enginePtr))
	{
		LOG_FATAL("Error setting up memory system! Aborting application.");
		return false;
	}

	// Logging
	if (!Logging_Initialize(g_enginePtr))
	{
		LOG_FATAL("Error setting up logger! Aborting application.");
		return false;
	}

	LOG_VERBOSE("Test");
	LOG_INFO("Test");
	LOG_DEBUG("Test");
	LOG_WARN("Test");
	LOG_ERROR("Test");
	LOG_FATAL("Test");

	// Event
	m_eventSystem = (Event_System*)m_memorySystem->Mem_Allocate(sizeof(Event_System));

	if (!m_eventSystem->EventSystem_Initialize(g_enginePtr))
	{
		LOG_FATAL("Error setting up event system! Aborting application.");
		return false;
	}

	// Register for Engine Level events...
	m_eventSystem->EventRegister(EVENT_TYPE_APPLICATION_QUIT, PFN_Application_Quit);
	m_eventSystem->EventRegister(EVENT_TYPE_INPUT_KEY_PRESSED, PFN_On_Key_Pressed);
	m_eventSystem->EventRegister(EVENT_TYPE_INPUT_KEY_RELEASED, PFN_On_Key_Released);

	// Input
	if (!Input_System::InputSystem_Initialize(g_enginePtr))
	{
		LOG_FATAL("Error setting up input system! Aborting application.");
		return false;
	}
	
	// Platform
	if (!IPlatform_Base::Platform_Initialize(g_enginePtr))
	{
		LOG_FATAL("Error setting up platform layer! Aborting application.");
		return false;
	}
	
	// Texture System
	if (!TextureSystem::TextureSystem_Initialize(g_enginePtr))
	{
		LOG_FATAL("Error setting up texture system! Aborting application.");
		return false;
	}

	// TODO: TEMP
	m_textureSystem->LoadTexture("../assets/Textures/texture.jpg", "texture");
	// TODO: END TEMP

	// Load VulkanRenderer.dll
	bool bLoadResult = m_PlatformLayer->Platform_LoadDLL("VulkanRenderer", &g_enginePtr->m_PlatformLayer->m_RendererLibrary);

	// Renderer
	if (!IRendererFrontend::RendererFrontend_Initialize(g_enginePtr, m_RenderingAPI))
	{
		LOG_FATAL("Error setting up renderer! Aborting application.");
		return false;
	}

	LOG_INFO("Number of logical processors: %llu", m_PlatformLayer->Platform_GetProcessorCoreCount());

	// Start the engine clock.
	//LOG_VERBOSE("Engine Clock started.");
	m_engineClock.Start();

	m_FPS = 0;

	return true;
}

bool IEngine::Engine_Run()
{
	while (!m_bApplicationQuitCalled)
	{
		m_PlatformLayer->Platform_ProcessMessages();
		if (!m_isSuspended)
		{
			g_enginePtr->m_PlatformLayer->Platform_Update();

			// Update Game Logic
			//g_enginePtr->m_gameLayer->GameUpdate();

			g_enginePtr->m_RendererFrontend->RendererFrontend_Update();

			g_enginePtr->m_inputSystem->InputSystem_Update();

			m_currentFrame++;
			m_FPS++;

			m_engineClock.Update();

			int time = m_engineClock.GetElapsedTime();
			if (time - m_lastTime >= 1)
			{
				m_lastTime = time;
				//LOG_VERBOSE("%llu", time);
				//LOG_VERBOSE("FPS: %llu, %llu", time, m_FPS);
				m_FPS = 0;
			}
		}
	}

	g_enginePtr->Engine_Shutdown();

	return true;
}

void IEngine::Engine_Shutdown()
{
	// Shutdown subsystems in reverse order.
	g_enginePtr->m_RendererFrontend->RendererFrontend_Shutdown();
	g_enginePtr->m_PlatformLayer->Platform_UnloadDLL(&g_enginePtr->m_PlatformLayer->m_RendererLibrary, "VulkanRenderer");
	
	g_enginePtr->m_textureSystem->TextureSystem_Shutdown();

	g_enginePtr->m_PlatformLayer->Platform_Shutdown(g_enginePtr);
	g_enginePtr->m_inputSystem->InputSystem_Shutdown(g_enginePtr);

	// Unregister from engine-level events
	g_enginePtr->m_eventSystem->EventUnregister(EVENT_TYPE_APPLICATION_QUIT, PFN_Application_Quit);
	g_enginePtr->m_eventSystem->EventUnregister(EVENT_TYPE_INPUT_KEY_PRESSED, PFN_On_Key_Pressed);
	g_enginePtr->m_eventSystem->EventUnregister(EVENT_TYPE_INPUT_KEY_RELEASED, PFN_On_Key_Released);

	g_enginePtr->m_eventSystem->EventSystem_Shutdown(g_enginePtr);

	LOG_INFO("Total Memory Allocations: %d", g_enginePtr->m_memorySystem->GetTotalAllocations());

	Logging_Shutdown();

	g_enginePtr->m_memorySystem->MemorySystem_Shutdown(g_enginePtr);

	return;

}

bool PFN_On_Key_Pressed(void* data)
{
	char msg = (char)data;
	LOG_DEBUG("%c Key Pressed", msg);

	if (g_enginePtr->m_inputSystem->IsKeyDown('M') && !g_enginePtr->m_inputSystem->WasKeyDown('M'))
	{
		LOG_VERBOSE("Total Memory Allocations: %d", g_enginePtr->m_memorySystem->GetTotalAllocations());
		return true;
	}
	return true;
}

bool PFN_On_Key_Released(void* data)
{
	char msg = (char)data;
	LOG_DEBUG("%c Key Released", msg);

	return true;
}