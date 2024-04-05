#pragma once

#include "Resources/Texture.h"

class IEngine;

#define DEFAULT_MAX_TEXTURES 256;

class TextureSystem
{
public:
	static bool TextureSystem_Initialize(IEngine* enginePtr);
	void TextureSystem_Shutdown();


	bool LoadTexture(const char* path, char* name);

	void UnloadTexture(const char* name);

	Texture m_RegisteredTextures[256];
};