#pragma once
#ifndef SCENE_HEADER
#define SCENE_HEADER
#include "../Graphics/Resources/Bindable.h"
#include "../Graphics/Resources/BindablesHeader.h"
#include "../Graphics/Resources/Drawable.h"
#include "../Graphics/Resources/DrawableArray.h"

#include <memory>
#include <vector>
#include <unordered_map>

#include "../Input/Camera.h"

class Model;

class Scene
{
	friend class FrameCommander;
	friend class ResourceManager;
	class PipeLineResources
	{
		struct RootSignatureResources
		{
			std::shared_ptr<RootSignature> pRootSignature;
			std::unordered_map<std::string, std::shared_ptr<DrawableArray>> DrawIndexed;
		};
	public:
		std::shared_ptr<PipelineStateObject> pPipeLineStateObject;
		std::unordered_map<std::string, RootSignatureResources> RootSignatures;
	};

public:
	Engine_API Scene() = default;
	Engine_API Scene(Scene&) = delete;
	Engine_API ~Scene() = default;
	Engine_API void AddDrawable(Drawable* pDrawable);
	Engine_API void AddModel(std::shared_ptr<Model> pModel);
	Engine_API void SetCamera(Camera* pCam);

private:
	void InitCamera();

private:
	std::unordered_map<std::string, PipeLineResources> DrawablesMap;

	Camera* pCamera = nullptr;
};

#endif