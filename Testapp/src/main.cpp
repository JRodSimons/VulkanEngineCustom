#include "SharedHeaders.h"
#include <GameClass.h>

int main()
{
	IEngine* Engine = new IEngine;

	bool result = Engine->Engine_Initialize();

	//Engine->m_gameLayer->GameInitialize(Engine);

	if (!result)
		return -1;

	result = Engine->Engine_Run();

	return 1;
}