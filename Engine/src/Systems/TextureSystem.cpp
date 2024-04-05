#include "PCH_Engine.h"

#include "TextureSystem.h"

#include "Core/Engine.h"
#include "Core/Memory.h"

#define STB_IMAGE_IMPLEMENTATION
#include "Vendor/stb_image.h"

static IEngine* g_enginePtr = nullptr;
static TextureSystem* g_TextureSys = nullptr;

bool TextureSystem::TextureSystem_Initialize(IEngine* enginePtr)
{
	g_enginePtr = enginePtr;
	g_enginePtr->m_textureSystem = (TextureSystem*)g_enginePtr->m_memorySystem->Mem_Allocate(sizeof(TextureSystem));
	g_TextureSys = g_enginePtr->m_textureSystem;

	// Invalidate the Registered Texture table.
	for (unsigned int i = 0; i < 256; i++)
	{
		g_TextureSys->m_RegisteredTextures[i].m_id = -1;
	}

	LOG_INFO("Texture System Initialized");

	return true;
}

void TextureSystem::TextureSystem_Shutdown()
{
	g_enginePtr->m_memorySystem->Mem_Free(g_enginePtr->m_textureSystem);
	g_enginePtr = 0;
}

bool TextureSystem::LoadTexture(const char* path, char* name)
{
	Texture newTexture;
	newTexture.m_pixelData = stbi_load(path, &newTexture.m_texWidth, &newTexture.m_texHeight, &newTexture.m_numChannels, STBI_rgb_alpha);

	if (!newTexture.m_pixelData)
	{
		LOG_ERROR("Unable to load texture data from '%s'.", path);
		return false;
	}

	newTexture.m_textureUse = TEXTURE_USE_DIFFUSE;	// TODO: Make configurable
	newTexture.m_name = name;

	newTexture.m_imageSize = newTexture.m_texWidth * newTexture.m_texHeight * 4;	// Channel count MUST be 4 for now
	
	for (unsigned int i = 0; i < 256; i++)
	{
		if (m_RegisteredTextures[i].m_id == -1)
		{
			m_RegisteredTextures[i] = newTexture;
			m_RegisteredTextures[i].m_id = i;
			break;
		}
	}

	// Pass data to the GPU for processing.
	//g_enginePtr->m_RendererFrontend->m_renderPacket.m_textures[0] = newTexture;
	//g_enginePtr->m_RendererFrontend->m_renderPacket.m_needsUpdate = true;

	//stbi_image_free(newTexture.m_pixelData);

	return true;
}

