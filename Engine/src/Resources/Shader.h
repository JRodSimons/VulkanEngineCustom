#pragma once

#include "Resources.h"
#include "EngineGlobals.h"

class ENGINE_API Shader : public BaseResource
{
public:

	bool CreateShader(const char* vertexShaderPath, const char* fragmentShaderPath);

	std::vector<unsigned int> LoadShaderFile(const char* filePath);

	std::vector<unsigned int> m_vertexShaderBytecode;
	std::vector<unsigned int> m_fragmentShaderBytecode;

private:
	char* m_vertexShaderName;
	char* m_fragmentShaderName;
};