#include "Application.h"
#include <Windows.h>

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	CoInitialize(NULL);

	Application & app = Application::getInstance();

	app.run();

	CoUninitialize();
	return 0;
}