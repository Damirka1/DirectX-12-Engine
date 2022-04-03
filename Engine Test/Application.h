#pragma once
#ifndef APPLICATION_HEADER
#define APPLICATION_HEADER
#include "../Engine/Headers/EngineCore.h"
#include "../Engine/Headers/Input/Camera.h"
#include "../Engine/Headers/Input/Keyboard.h"
#include "../Engine/Headers/Input/Mouse.h"

#include "../Engine/Headers/Scene/Scene.h"
#include "../Engine/Headers/Scene/Components/StaticMeshComponent.h"




class Application
{
public:
	Application(HINSTANCE hInstance);
	void Run();
	~Application();

private:
	EngineCore* Core;

	std::shared_ptr<Keyboard> kb;
	std::shared_ptr<Mouse> ms;
	std::shared_ptr<Scene> pScene;

	Camera* pCamera;
	std::vector<std::shared_ptr<StaticMeshComponent>> models;
};

#endif

