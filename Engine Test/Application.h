#pragma once
#ifndef APPLICATION_HEADER
#define APPLICATION_HEADER

#include "..\Engine\Headers\Window.h"
#include "..\Engine\Headers\ResourceManager.h"
#include "..\Engine\Headers\Graphics.h"
#include "..\Engine\Models\Triangle.h"
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
	Triangle* t;
	Keyboard* k;
	Mouse* m;
};

#endif

