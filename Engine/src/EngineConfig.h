// --- This header file holds the engine configuration struct created by the .EXE and used to initialize the engine --- //
// TODO: This is most likely temporary...

#pragma once

#include "EngineGlobals.h"

typedef struct engineConfig
{
	bool (*game_initialize)(engineConfig* config);

	bool (*game_update) ();

	void (*game_shutdown)();

	const char* ApplicationName;
	int Start_Pos_X;	// TODO: can these be unsigned?? (and 8-bit?)
	int Start_Pos_Y;
	unsigned int Width;
	unsigned int Height;
};