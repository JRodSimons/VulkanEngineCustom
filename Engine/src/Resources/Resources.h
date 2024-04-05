#pragma once

#include "Filesystem.h"

enum e_ResourceTypes
{
	RESOURCE_TYPE_UNDEFINED = 0,

	RESOURCE_TYPE_TEXTURE,
	RESOURCE_TYPE_MATERIAL,
	RESOURCE_TYPE_MODEL,
	RESOURCE_TYPE_SHADER
};

class BaseResource
{
public:
	unsigned int m_id;
	char* m_name;
	e_ResourceTypes m_type;
};