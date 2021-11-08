#pragma once
#ifndef RESOURCE_MANAGER_HEADER
#define RESOURCE_MANAGER_HEADER
#include "Graphics/Resources/Bindable.h"
#include "Graphics/Resources/BindablesHeader.h"
#include "Graphics/Resources/Drawable.h"
#include "Graphics/Resources/DrawableArray.h"
#include "Events/ScriptManager.h"
#include <unordered_map>
#include <string>
#include <memory>
#include <queue>

class Window;
class Scene;

class ResourceManager
{
	friend class FrameCommander;
public:
	Engine_API ResourceManager(Window* pWindow) noexcept;
	ResourceManager(ResourceManager&) = delete;

	Engine_API std::shared_ptr<VertexBuffer> CreateVertexBuffer(Drawable* pDrawable, const void* pData, const unsigned int Stride, unsigned int DataSize, VertexLayout& Lay, unsigned int VertexCount, bool unique = false, unsigned int Slot = 0) noexcept;
	Engine_API std::shared_ptr<IndexBuffer> CreateIndexBuffer(Drawable* pDrawable, std::vector<unsigned int>* Indecies) noexcept;
	Engine_API std::shared_ptr<RootSignature> CreateRootSignature(Drawable* pDrawable, std::string& PSO_Key, RS_Layout& Lay) noexcept;
	Engine_API std::shared_ptr<PipelineStateObject> CreatePipelineStateObject(PSO_Layout& pLay, VertexLayout& vLay) noexcept;
	Engine_API std::shared_ptr<ConstantBuffer> CreateConstBuffer(Drawable* pDrawable, const void* pData, unsigned int DataSize, UINT RootParam, UINT Range, UINT RangeIndex) noexcept;
	Engine_API std::shared_ptr<Texture2D> CreateTexture2D(Drawable* pDrawable, const std::string& Path, UINT RootParam, UINT Range, UINT RangeIndex, bool OnlyPixelShader = false);
	Engine_API std::shared_ptr<Sampler> CreateDefaultSampler(Drawable* pDrawable, UINT RootParam, UINT Range, UINT RangeIndex) noexcept;
	
	Engine_API void InitializeResourcesOfScene(Scene* pScene);
	Engine_API ~ResourceManager();

private:
	Graphics* pGraphics;
	GlobalHeap Heap;

	std::unordered_map<std::string, std::shared_ptr<Bindable>> Bindables;
	std::unordered_map<std::string, std::shared_ptr<Resource>> Resources;
};

#endif
