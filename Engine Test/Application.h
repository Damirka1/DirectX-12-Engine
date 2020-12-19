#pragma once
#ifndef APPLICATION_HEADER
#define APPLICATION_HEADER

#include "..\Engine\Headers\Window.h"
#include "..\Engine\Headers\FrameCommander.h"
#include "..\Engine\Models\Triangle.h"
#include "..\Engine\Models\Cube.h"
#include "..\Engine\Models\Rectangle.h"
#include "..\Engine\Headers\Input\Keyboard.h"
#include "..\Engine\Headers\Input\Mouse.h"
#include "..\Engine\Headers\Input\Camera.h"

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
	ResourceManager* RM;
	FrameCommander* FC;
	std::vector<Cube*> Cubes;
	Rect* r;
	Keyboard* k;
	Mouse* m;
	Camera* cam;
};

#endif

