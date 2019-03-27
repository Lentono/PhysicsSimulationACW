#include <Windows.h>
#include "System.h"

//Windows entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow) {
	
	auto* system = new System();

	if (system->GetInitializationState())
	{
		return 0;
	}

	system->Run();

	//Release system object
	delete system;
	system = nullptr;

	return 0;
}