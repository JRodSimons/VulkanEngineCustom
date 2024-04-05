#pragma once

#include <GameClass.h>

class GameLayer : public IGame
{
	bool GameInitialize(IEngine* enginePtr) const override;
	void GameShutdown() const override;

	bool GameUpdate() const override;
};