#pragma once
#ifndef APPLICATION_HEADER
#define APPLICATION_HEADER

#include "..\Engine\Headers\Window.h"
#include "..\Engine\Headers\FrameCommander.h"
#include "..\Engine\Models\Triangle.h"
#include "..\Engine\Models\Cube.h"
#include "..\Engine\Headers\Input\Keyboard.h"
#include "..\Engine\Headers\Input\Mouse.h"

#include "..\ConsoleDLL\Console.h"


class Application
{
public:
	Application(HINSTANCE hInstance);
	void Run();
	~Application();

private:
	Console Con;

	Window* pWindow;
	Graphics* gtx;
	ResourceManager* RM;
	FrameCommander* FC;
	std::vector<Cube*> Cubes;
	Keyboard* k;
	Mouse* m;
};

#endif

