#include "pch.h"
#include "App.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	App testme;

	return testme.Start();
}