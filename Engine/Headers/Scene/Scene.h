#pragma once
#ifndef SCENE_HEADER
#define SCENE_HEADER

#include <memory>
#include <vector>
#include <unordered_map>


#include "../Input/Camera.h"
#include "Components/StaticMeshComponent.h"

class Model;

class Scene
{
	friend class FrameCommander;
	friend class ResourceManager;

public:
	Engine_API Scene() = default;
	Engine_API Scene(Scene&) = delete;
	Engine_API ~Scene() = default;
	//Engine_API void AddDrawable(Drawable* pDrawable);
	//Engine_API void AddModel(std::shared_ptr<Model> pModel);
	Engine_API void AddModel(std::shared_ptr<StaticMeshComponent> pModel);

	Engine_API void AddModels(std::vector<std::shared_ptr<StaticMeshComponent>> Models);

	Engine_API void Update();

	Engine_API void SetCamera(Camera* pCam);

private:
	//void InitCamera();

private:
	std::vector<std::shared_ptr<StaticMeshComponent>> Models;
	Camera* pCamera = nullptr;
};

#endif