#pragma once
#ifndef SCENE_HEADER
#define SCENE_HEADER

#include <memory>
#include <vector>
#include <unordered_map>

#include "../Input/Camera.h"
#include "Components/StaticMeshComponent.h"
#include "SceneResources.h"

#include "Objects/Plane.h"
#include "Objects/Sphere.h"
#include "Objects/Cube.h"

class Model;

class Scene
{
	friend class FrameCommander;
	friend class ResourceManager;

public:
	Engine_API Scene(Graphics* pGraphics);
	Engine_API Scene(Scene&) = delete;
	Engine_API ~Scene() = default;
	//Engine_API void AddDrawable(Drawable* pDrawable);
	//Engine_API void AddModel(std::shared_ptr<Model> pModel);
	/*Engine_API void AddModel(std::shared_ptr<StaticMeshComponent> pModel);

	Engine_API void AddModels(std::vector<std::shared_ptr<StaticMeshComponent>> Models);*/

	Engine_API void Update();

	Engine_API void SetCamera(Camera* pCam);

	// TODO: finish memory load system!
	// TODO: make this func thread safe!
	Engine_API std::shared_ptr<StaticMeshComponent> LoadModel(std::string Path, std::string Tag, float Scale = 1.0f);

	Engine_API std::shared_ptr<Plane> CreatePlane(DirectX::XMFLOAT3 Pos = { 0, 0, 0 }, DirectX::XMFLOAT3 Rotation = { 0, 0, 0 });

	Engine_API std::shared_ptr<Sphere> CreateSphere(DirectX::XMFLOAT3 Pos = { 0, 0, 0 }, DirectX::XMFLOAT3 Rotation = { 0, 0, 0 });

	Engine_API std::shared_ptr<Cube> CreateCube(DirectX::XMFLOAT3 Pos = { 0, 0, 0 }, DirectX::XMFLOAT3 Rotation = { 0, 0, 0 });
	

private:
	void InitializeResources();

private:
	std::vector<std::shared_ptr<StaticMeshComponent>> Models;
	Camera* pCamera = nullptr;

	SceneResources SceneResources;
};

#endif