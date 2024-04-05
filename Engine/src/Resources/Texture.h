#pragma once

#include "Resources.h"

enum TextureUse
{
	TEXTURE_USE_UNKNOWN = 0,
	TEXTURE_USE_DIFFUSE = 1,
};

class Texture : public BaseResource
{
public:
	Texture()
	{
		m_type = RESOURCE_TYPE_TEXTURE;
		m_pixelData = 0;
		m_textureUse = TEXTURE_USE_UNKNOWN;

		m_texWidth = 0;
		m_texHeight = 0;
		m_numChannels = 0;
		m_imageSize = 0;
	}

	unsigned char* m_pixelData;
	TextureUse m_textureUse;

	int m_texWidth;
	int m_texHeight;
	int m_numChannels;

	int m_imageSize;
};