#include "PCH_Engine.h"

#include "Renderer_Interface.h"
#include "Core/Engine.h"
#include "Core/Memory.h"

#include "Platform/Platform.h"

#include "Resources/Texture.h"

static IEngine* g_enginePtr = 0;
static IRendererFrontend* g_rendererInterface = 0;

bool IRendererFrontend::RendererFrontend_Initialize(IEngine* enginePtr, renderingAPIChoices API)
{
	g_enginePtr = enginePtr;
	g_rendererInterface = (IRendererFrontend*)g_enginePtr->m_memorySystem->Mem_Allocate(sizeof(IRendererFrontend));
	g_enginePtr->m_RendererFrontend = g_rendererInterface;

	if (!g_enginePtr->m_RendererFrontend->RendererBackend_Initialize(API))
	{
		return false;
	}

	return true;
}

bool IRendererFrontend::RendererBackend_Initialize(renderingAPIChoices API)
{
	bool result = false;

	switch (API)
	{
	case API_VULKAN:
		g_enginePtr->m_PlatformLayer->Platform_LoadDLLFunction(&g_enginePtr->m_PlatformLayer->m_RendererLibrary, "VulkanRendererInitialize", &g_enginePtr->m_PlatformLayer->m_rendererLibraryInitialize);

		result = g_enginePtr->m_PlatformLayer->m_rendererLibraryInitialize(g_enginePtr);
		if (!result)
		{
			LOG_ERROR("Vulkan Renderer failed to initialize!");
			return false;
		}
		break;
	case API_D3D12:
		if (!result)
		{
			LOG_ERROR("D3D12 Renderer not yet created, coming soon!");
			return false;
		}
		LOG_ERROR("D3D12 Renderer not yet created, coming soon!");
		break;
	case API_UNDEFINED:
	default:
		result = false;
		LOG_ERROR("Rendering API MUST be defined in the Engine Config! Shutting down");
		return false;
		break;
	}

	m_renderPacket.m_needsUpdate = false;

	return true;
}

void IRendererFrontend::RendererFrontend_Update()
{
	g_rendererInterface->RendererBackend_Update();
}

void IRendererFrontend::RendererBackend_Update()
{
	if (m_renderPacket.m_needsUpdate == true)
	{
		m_RendererBackend->RendererBackend_Update(m_renderPacket);
		m_renderPacket.m_needsUpdate = false;
	}
	else
	{
		m_RendererBackend->RendererBackend_Update(m_renderPacket);
	}
}

void IRendererFrontend::RendererFrontend_Shutdown()
{
	g_rendererInterface->RendererBackend_Shutdown();

	g_enginePtr->m_memorySystem->Mem_Free(g_rendererInterface);

	g_enginePtr = 0;
	g_rendererInterface = 0;
}

void IRendererFrontend::RendererBackend_Shutdown()
{
	m_RendererBackend->RendererBackend_Shutdown();

	//delete m_RendererBackend;
}