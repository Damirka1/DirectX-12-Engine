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

	//class PipeLineResources
	//{
	//	struct RootSignatureResources
	//	{
	//		std::shared_ptr<RootSignature> pRootSignature;
	//		//std::unordered_map<std::string, std::shared_ptr<DrawableArray>> DrawArrayIndexed;
	//		//std::unordered_map<std::string, std::shared_ptr<Drawable>>		DrawIndexed;
	//	};
	//public:
	//	std::shared_ptr<PipelineStateObject> pPipeLineStateObject;
	//	std::unordered_map<std::string, RootSignatureResources> RootSignatures;
	//};

public:
	Engine_API Scene() = default;
	Engine_API Scene(Scene&) = delete;
	Engine_API ~Scene() = default;
	//Engine_API void AddDrawable(Drawable* pDrawable);
	//Engine_API void AddModel(std::shared_ptr<Model> pModel);
	Engine_API void AddModel(std::shared_ptr<StaticMeshComponent> pModel);

	Engine_API void SetCamera(Camera* pCam);

private:
	//void InitCamera();

private:
	std::vector<std::shared_ptr<StaticMeshComponent>> Models;
	Camera* pCamera = nullptr;
};

#endif