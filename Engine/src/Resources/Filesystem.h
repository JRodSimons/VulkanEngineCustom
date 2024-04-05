#pragma once

#include "EngineGlobals.h"

#include <stdio.h>
#include <vector>

class File_t
{
public:
	void* m_handle;
	const char* m_path;
	bool  m_open;
	char m_AccessMode;
};

enum e_FileAccessMode
{
	FILE_ACCESS_MODE_READ,
	FILE_ACCESS_MODE_WRITE,
	FILE_ACCESS_MODE_READ_BINARY,
	FILE_ACCESS_MODE_WRITE_BINARY,

	FILE_ACCESS_MODE_MAX
};

class ENGINE_API FileSystem
{
public:
	const char* GetBaseAssetPath();	// TODO: Make configurable

	static bool				OpenFile(const char* path, e_FileAccessMode mode, File_t* out_file_handle);
	static void				CloseFile(File_t* file_handle);

	static int				GetFileSize(File_t* file);
	static void				ReadBinaryStreamFromFile(File_t* file, bool startAtEnd, unsigned int* out_bytes_read, std::vector<unsigned int>& out_bytes);
	static void				WriteLineToFile(char* str, File_t* file);

private:
	
};
