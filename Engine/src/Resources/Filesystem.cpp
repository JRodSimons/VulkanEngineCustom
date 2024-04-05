#include "PCH_Engine.h"

#include "Filesystem.h"
#include "Core/String.h"


const char* FileAccessModeStr[FILE_ACCESS_MODE_MAX] = {
	"r",
	"w",
	"rb",
	"wb"
};

const char* FileSystem::GetBaseAssetPath()
{
	return "../assets/";
}

bool FileSystem::OpenFile(const char* path, e_FileAccessMode mode, File_t* out_file_handle)
{
	File_t File;
	File.m_open = false;

	File.m_handle = fopen(path, FileAccessModeStr[mode]);

	if (File.m_handle != NULL)
	{
		File.m_path = path;
		File.m_open = true;
		*out_file_handle = File;
		return true;
	}
	else
	{
		LOG_ERROR("Filesystem failed to open file: %s", File.m_path);
		return false;
	}
}

void FileSystem::CloseFile(File_t* file_handle)
{
	if (!file_handle || !file_handle->m_open)
	{
		LOG_ERROR("Filesystem cannot close file: %s. No open file was found!.", file_handle->m_path);
	}

	int result = fclose((FILE*)file_handle->m_handle);

	if (result != 0)
	{
		LOG_ERROR("There was an error attempting to close file: %s", file_handle->m_path);
	}
	else
	{
		file_handle->m_open = false;
		file_handle->m_path = 0;
	}
}

int FileSystem::GetFileSize(File_t* file)
{
	if (!file || !file->m_open)
	{
		LOG_ERROR("GetFileSize: File not found! (or not opened)");
		return -1;
	}

	fseek((FILE*)file->m_handle, 0, SEEK_END);

	int result = ftell((FILE*)file->m_handle);

	rewind((FILE*)file->m_handle);

	//CloseFile(file);

	return result;
}

void FileSystem::ReadBinaryStreamFromFile(File_t* file, bool startAtEnd, unsigned int* out_bytes_read, std::vector<unsigned int>& out_bytes)
{
	if (!file->m_open)
	{
		LOG_ERROR("File %s has not been opened, read failed.", file->m_path);
		return;
	}

	int fileLength = GetFileSize(file);

	*out_bytes_read = fread(out_bytes.data(), 1, fileLength, (FILE*)file->m_handle);
}

void FileSystem::WriteLineToFile(char* str, File_t* file)
{
	if (!file || !file->m_open)
		return;

	char* message = string_format("%s\n", str, 0);

	int result = fputs(message, (FILE*)file->m_handle);

	if (result < 0)
	{
		LOG_ERROR("WriteStreamToFile failed()");
	}
}