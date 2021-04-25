#pragma once
#ifndef SCRIPTMANAGER_HEADER
#define SCRIPTMANAGER_HEADER
#include "EventListener.h"

class Drawable;
class Window;

class ScriptManager
{
public:
	ScriptManager(Window* pWindow);
	~ScriptManager();

	void AddDrawable(Drawable* pDrawable);

	void StartListen();

private:
	static void ListenMouseAndKeyboardEvents(ScriptManager* This);
	static void TreatScripts(ScriptManager* This);

private:
	Window* pWindow;
	std::vector<Drawable*> pDrawables;

	// Threads.
	HANDLE ListenEvents;
	HANDLE Scripts;
};

#endif