#pragma once
#ifndef ENGINE_HEADER
#define ENGINE_HEADER
#include "Window.h"
#include "Graphics.h"
#include "ResourceManager.h"
#include "FrameCommander.h"
#include "PhysX/PhysxManager.h"
//#include "Events/ScriptManager.h"
#include "../Console/Console.h"

#include "Timer.h"
#include "Input/Camera.h"
#include "Input/Mouse.h"
#include "Input/Keyboard.h"


#include "Scene/Components/StaticMeshComponent.h"

#include "Scene/Objects/Plane.h"
#include "Scene/Objects/Sphere.h"
#include "Scene/Objects/Cube.h"


class EngineCore
{
public:
	// TODO: Add settings as parameter
	EngineCore(HINSTANCE hInstance)
	{
		// Create core objects
		_Window = new Window(hInstance, L"DirectX 12 Engine", 1280, 720);
		//_ResourceManager = new ResourceManager(_Window->GetGraphics());
		
		//_PhysxManager = new PhysxManager();

		// _ScriptManager = new ScriptManager(_Window);
		_Timer = new Timer();

		_FrameCommander = new FrameCommander(_Window, _Timer);

		// Get input from window
		_Keyboard = _Window->GetKeyboard();
		_Mouse = _Window->GetMouse();

		Running = true;
	}

	bool IsRunning()
	{
		return Running;
	}

	bool WindowIsExist()
	{
		return _Window->IsExist();
	}

	void ShowWindow()
	{
		_Window->Show();
	}

	void HideWindow()
	{
		_Window->Hide();
	}

	void EnableCursor()
	{
		_Window->EnableCursor();
	}

	void DisableCursor()
	{
		_Window->DisableCursor();
	}

	void SetBackgroundColor(float r, float g, float b)
	{
		_FrameCommander->SetBackgroundColor(r, g, b);
	}

	void SetCurrentScene(std::shared_ptr<Scene> Scene)
	{
		_FrameCommander->SetScene(std::move(Scene));
	}

	// Return elapsed time from start window.				 
	Engine_API float TimerPeek() const noexcept
	{
		return _Timer->Peek();
	}

	// Return elapsed time from frame render.				 
	Engine_API float TimerMark() const noexcept
	{
		return _Timer->Mark();
	}

	void PrepareDX()
	{
		_FrameCommander->InitializeResources();
		_Timer->Mark();
	}

	void SetupJobs()
	{
		_Window->ProcessMessages();
		_FrameCommander->SetupInit();
		//float dt = _Timer->Mark();

		//_PhysxManager->Simulate(dt);
	}

	void ExecuteJobs()
	{
		_FrameCommander->InitializeResources();
		_FrameCommander->Render();
	}

	void Update() {
		_FrameCommander->Update();
	}

	void CreateConsole()
	{
		_Console = new Console(L"Engine debug console");
	}

	void PrintToConsole(std::wstring Msg)
	{
		if (_Console)
			_Console->Write(Msg.c_str());
	}

	void PrintGraphicsInfoToConsole()
	{
		if (_Console)
			_Console->Write(_Window->GetGraphics()->GetInfo().c_str());
	}

	// TODO: add some params.
	std::shared_ptr<Scene> CreateScene(std::string Tag = "Scene")
	{
		std::shared_ptr<Scene> _Scene = std::make_shared<Scene>(_Window->GetGraphics());
		// TODO: add tag to scene.
		
		return _Scene;
	}

	//void RemoveModelByTag(std::string Tag);
	//void RemoveModelByPtr(std::shared_ptr<Model> Model);

	std::shared_ptr<Keyboard> GetKeyboardInput()
	{
		return _Keyboard;
	}

	std::shared_ptr<Mouse> GetMouseInput()
	{
		return _Mouse;
	}

	~EngineCore()
	{
		delete _Console;
		delete _Window;
		//delete _ResourceManager;
		delete _FrameCommander;
		//delete _PhysxManager;
		//delete _ScriptManager;
		delete _Timer;
	}

private:
		// Engine core:
		Console* _Console;
		Window* _Window;
		//ResourceManager* _ResourceManager;
		//PhysxManager* _PhysxManager;
		FrameCommander* _FrameCommander;
		//ScriptManager* _ScriptManager;

		Timer* _Timer;

		bool Running = false;

		// Engine input:
		std::shared_ptr<Keyboard> _Keyboard;
		std::shared_ptr<Mouse> _Mouse;

		// Objects:
		// TODO: don't forget to add els to this vectors
		std::vector<std::shared_ptr<Scene>> _Scenes;


		// TODO: make jobs system
};



#endif
