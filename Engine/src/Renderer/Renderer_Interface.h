#pragma once

#include "EngineGlobals.h"

#include "Resources/Texture.h"
#include "Resources/Shader.h"
#include <vector>

class Texture;
class IEngine;
class IRendererFrontend;
class IGenericRenderer;

struct RenderPacket_t
{
	Texture m_textures[256];

	bool m_needsUpdate;
};

enum renderingAPIChoices
{
	API_UNDEFINED = 0, // In the case of not defining an API in the config. Should we just default to Vulkan in this case?

	API_VULKAN,
	API_D3D12
};

class IRendererFrontend
{
public:
	static bool RendererFrontend_Initialize(IEngine* enginePtr, renderingAPIChoices API);
	void		RendererFrontend_Update();
	void		RendererFrontend_Shutdown();

private:
	bool RendererBackend_Initialize(renderingAPIChoices API);
	void RendererBackend_Update();
	void RendererBackend_Shutdown();

public:
	IGenericRenderer* m_RendererBackend;

	RenderPacket_t		m_renderPacket;

private:
	renderingAPIChoices m_API = API_UNDEFINED;
};

class IGenericRenderer
{
public:
	virtual bool		RendererBackend_Initialize(IEngine* enginePtr) = 0;
	virtual void		RendererBackend_Update(RenderPacket_t renderPacket) = 0;
	virtual void		RendererBackend_Shutdown() = 0;
	virtual void		RendererBackend_DrawFrame() = 0;

	virtual bool		RendererBackend_ShaderCreate(Shader shader) = 0;
};