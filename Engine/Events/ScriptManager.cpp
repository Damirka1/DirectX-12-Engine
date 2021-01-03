#include "..\Headers\Events\ScriptManager.h"
#include "..\Headers\Graphics\Resources\Drawable.h"
#include "..\Headers\Window.h"
#include "..\Headers\Input\Mouse.h"
#include "..\Headers\Input\Keyboard.h"

ScriptManager::ScriptManager(Window* pWindow)
	:
	pWindow(pWindow)
{

}

ScriptManager::~ScriptManager()
{
	WaitForSingleObject(ListenEvents, INFINITE);
	CloseHandle(ListenEvents);

	WaitForSingleObject(Scripts, INFINITE);
	CloseHandle(Scripts);
}

void ScriptManager::AddDrawable(Drawable* pDrawable)
{
	pDrawables.push_back(pDrawable);
}

void ScriptManager::StartListen()
{
	ListenEvents = CreateThread(nullptr, NULL, (LPTHREAD_START_ROUTINE)ListenMouseAndKeyboardEvents, this, NULL, nullptr);
	Scripts = CreateThread(nullptr, NULL, (LPTHREAD_START_ROUTINE)TreatScripts, this, NULL, nullptr);
}

void ScriptManager::ListenMouseAndKeyboardEvents(ScriptManager* This)
{
	while (This->pWindow->Exist)
	{
		if (This->pWindow->Visible)
		{
			// Process mouse events.
			auto& MouseEvents = This->pWindow->pMouse->EventsForWindow;
			while (MouseEvents.size() > 0)
			{
				auto ev = MouseEvents.front();
				MouseEvents.pop();

				for (auto& Object : This->pDrawables)
				{
					for (auto& Listener : Object->EventListeners)
					{
						Listener->ListenMouseEvents(Object, &ev, This->pWindow);
					}
				}
			}

			// Process keyboard events.
			auto& KeyboardEvents = This->pWindow->pKeyboard->EventsForWindow;
			while (KeyboardEvents.size() > 0)
			{
				auto ev = KeyboardEvents.front();
				KeyboardEvents.pop();

				for (auto& Object : This->pDrawables)
				{
					for (auto& Listener : Object->EventListeners)
					{
						Listener->ListenKeyboardEvents(Object, &ev, This->pWindow);
					}
				}
			}
		}

		// TO DO : dynamic calculation sleep time.
		Sleep(10);
	}
}

void ScriptManager::TreatScripts(ScriptManager* This)
{
	while (This->pWindow->Exist)
	{
		for (auto& Object : This->pDrawables)
		{
			for (auto& Listener : Object->EventListeners)
			{
				Listener->Script(Object, This->pWindow);
			}
		}
		// TO DO : dynamic calculation sleep time.
		Sleep(10);
	}
}
