#include "PCH_Engine.h"

#include "Shader.h"

#include "Core/Logging.h"
#include "Core/Memory.h"
#include "Resources/Filesystem.h"

bool Shader::CreateShader(const char* vertexShaderPath, const char* fragmentShaderPath)
{
	m_type = RESOURCE_TYPE_SHADER;

	m_vertexShaderBytecode = LoadShaderFile(vertexShaderPath);
	m_fragmentShaderBytecode = LoadShaderFile(fragmentShaderPath);

	return true;
}

std::vector<unsigned int> Shader::LoadShaderFile(const char* filePath)
{
	File_t fileHandle;

	FileSystem::OpenFile(filePath, FILE_ACCESS_MODE_READ_BINARY, &fileHandle);

	int fileSize = FileSystem::GetFileSize(&fileHandle);

	std::vector<unsigned int> byteCode(fileSize);

	unsigned int outBytesRead = 0;

	FileSystem::ReadBinaryStreamFromFile(&fileHandle, true, &outBytesRead, byteCode);

	FileSystem::CloseFile(&fileHandle);

	if (outBytesRead != byteCode.size())
	{
		LOG_ERROR("Error loading shader module: %s", filePath);
	}

	return byteCode;
}