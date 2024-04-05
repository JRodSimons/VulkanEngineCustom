#pragma once

#include <cassert>
#include <stdexcept>

#define AssertMsg(_msg)					\
	if (_msg)							\
		throw std::runtime_error(_msg);	\

class IEngine;

bool Logging_Initialize(IEngine* enginePtr);
void Logging_Shutdown();

enum Log_Level
{
	LOG_LEVEL_VERBOSE = 0,
	LOG_LEVEL_INFO,
	LOG_LEVEL_DEBUG,
	LOG_LEVEL_WARN,
	LOG_LEVEL_ERROR,
	LOG_LEVEL_FATAL
};

ENGINE_API void Log_Message(Log_Level level, const char* message, ...);

#define LOG_VERBOSE(message, ...) (Log_Message(LOG_LEVEL_VERBOSE, message, __VA_ARGS__))
#define LOG_INFO(message, ...) (Log_Message(LOG_LEVEL_INFO, message, __VA_ARGS__))
#define LOG_DEBUG(message, ...) (Log_Message(LOG_LEVEL_DEBUG, message, __VA_ARGS__))
#define LOG_WARN(message, ...) (Log_Message(LOG_LEVEL_WARN, message, __VA_ARGS__))
#define LOG_ERROR(message, ...) (Log_Message(LOG_LEVEL_ERROR, message, __VA_ARGS__))
#define LOG_FATAL(message, ...) (Log_Message(LOG_LEVEL_FATAL, message, __VA_ARGS__))

// ANSI color codes

// --- (adapted from https://gist.github.com/RabaDabaDoba/145049536f815903c79944599c6f952a) --- //
#define ANSI_BLK \33[30m
#define ANSI_RED \33[31m
#define ANSI_GRN \33[32m
#define ANSI_YEL \33[33m
#define ANSI_BLU \33[34m
#define ANSI_MAG \33[35m
#define ANSI_CYN \33[36m
#define ANSI_WHT \33[37m

#define ANSI_BLKB \33[40m
#define ANSI_REDB \33[41m
#define ANSI_GRNB \33[42m
#define ANSI_YELB \33[43m
#define ANSI_BLUB \33[44m
#define ANSI_MAGB \33[45m
#define ANSI_CYNB \33[46m
#define ANSI_WHTB \33[47m

#define ANSI_RESET \33[37m\33[40m