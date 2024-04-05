#include "Game.h"

#include "Core/Engine.h"
#include "Core/Logging.h"

IEngine* g_enginePtr = 0;

bool GameLayer::GameInitialize(IEngine* enginePtr) const
{
	LOG_INFO("GameInitialize() called");

	g_enginePtr = enginePtr;
	return true;
}