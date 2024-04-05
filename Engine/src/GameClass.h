/*
*
* This file contains the skeleton structure for a game to be used by the game library.
*
*
*/
#pragma once

#include "EngineGlobals.h"

class IEngine;

ENGINE_API class IGame
{
public:
	virtual bool GameInitialize(IEngine* enginePtr) const = 0;
	virtual void GameShutdown() const = 0;

	virtual bool GameUpdate() const = 0;

private:
	const char* m_GameName;
};