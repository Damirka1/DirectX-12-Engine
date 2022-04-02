#pragma once
#ifndef ENGINE_HEADER
#define ENGINE_HEADER
#include "../Console/Console.h"
#include "ResourceManager.h"
#include "FrameCommander.h"
#include "Scene/Model.h"


class EngineCore
{
public:
	// TODO: Add settings as parameter
	EngineCore(HINSTANCE hInstance)
	{
		// Create core objects
		_Window = new Window(hInstance, L"DirectX 12 Engine", 1280, 720);
		_ResourceManager = new ResourceManager(_Window);
		_FrameCommander = new FrameCommander(_Window, _ResourceManager);

		// _ScriptManager = new ScriptManager(_Window);
		_Timer = new Timer();

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
	}

	void SetupJobs()
	{
		_Window->ProcessMessages();
		_FrameCommander->SetupInit();
	}

	void ExecuteJobs()
	{
		_FrameCommander->InitializeResources();
		_FrameCommander->Render();
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
		std::shared_ptr<Scene> _Scene = std::make_shared<Scene>();
		// TODO: add tag to scene.
		
		return _Scene;
	}

	// TODO: finish memory load system!
	// TODO: make this func thread safe!
	std::shared_ptr<Model> LoadModel(std::string Path, std::string Tag, float Scale = 1.0f)
	{
		std::shared_ptr<Model> _Model = std::make_shared<Model>(_ResourceManager, Path, Scale);
		// TODO: add tag to model

		return _Model;
	}

	void RemoveModelByTag(std::string Tag);
	void RemoveModelByPtr(std::shared_ptr<Model> Model);



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
		delete _ResourceManager;
		delete _FrameCommander;
		delete _ScriptManager;
		delete _Timer;
	}

private:
		// Engine core:
		Console* _Console;
		Window* _Window;
		ResourceManager* _ResourceManager;
		FrameCommander* _FrameCommander;
		ScriptManager* _ScriptManager;

		Timer* _Timer;

		bool Running = false;


		// Engine input:
		std::shared_ptr<Keyboard> _Keyboard;
		std::shared_ptr<Mouse> _Mouse;

		// Objects:
		// TODO: don't forget to add els to this vectors
		std::vector<std::shared_ptr<Scene>> _Scenes;
		std::vector<std::shared_ptr<Model>> _Models;


		// TODO: make jobs system
};



#endif
