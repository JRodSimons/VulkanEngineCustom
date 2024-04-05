#include "PCH_Engine.h"
#include "Logging.h"

#include <stdarg.h>

#include "Core/Engine.h"
#include "Core/String.h"
#include "Platform/Platform.h"

#include "Resources/Filesystem.h"

static IEngine* g_enginePtr = 0;

File_t ConsoleLog;

const char* Log_Level_Strings[6] =
{
	"[VERBOSE]",
	"[INFO]\t ",
	"[DEBUG]\t ",
	"[WARN]\t ",
	"[ERROR]\t ",
	"[FATAL]\t "
};

bool Logging_Initialize(IEngine* enginePtr)
{
	FileSystem::OpenFile("../Console.log", FILE_ACCESS_MODE_WRITE, &ConsoleLog);

	g_enginePtr = enginePtr;

	return true;
}

void Logging_Shutdown()
{
	FileSystem::CloseFile(&ConsoleLog);
}

void Log_Message(Log_Level level, const char* message, ...)
{
	char messageBuffer[10000];
	char outMessage[10000];

	sprintf(messageBuffer, "%s\0", message);

	va_list va_args;
	va_start(va_args, message);
	vsprintf(outMessage, messageBuffer, va_args);
	va_end(va_args);

	// Append log level string to the message
	char* outMessage2 = string_format("%s%s\n", Log_Level_Strings[level], outMessage);

	g_enginePtr->m_PlatformLayer->Platform_LogMessage(level, outMessage2);
}

