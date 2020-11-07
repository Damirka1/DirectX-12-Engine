#pragma once
#ifndef APPLICATION_HEADER
#define APPLICATION_HEADER

#include "..\Engine\Headers\Window.h"
#include "..\Engine\Headers\ResourceManager.h"
#include "..\Engine\Headers\Graphics.h"
#include "..\Engine\Models\Triangle.h"
#ifdef _DEBUG
#include "..\ConsoleDLL\Console.h"
#endif // DEBUG


class Application
{
public:
	Application(HINSTANCE hInstance);
	void Run();
	~Application();

private:
#ifdef _DEBUG
	Console* Con;
#endif
	Window* pWindow;
	Graphics* gtx;
	ResourceManager* RM;
	Triangle* t;
};

#endif

