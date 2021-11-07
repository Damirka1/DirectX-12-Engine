#pragma once
#ifndef RESOURCE_MANAGER_HEADER
#define RESOURCE_MANAGER_HEADER
#include "Graphics\Resources\Bindable.h"
#include "Graphics\Resources\BindablesHeader.h"
#include "Graphics\Resources\Drawable.h"
#include "Graphics\Resources\DrawableArray.h"
#include "Events\ScriptManager.h"
#include <unordered_map>
#include <string>
#include <memory>
#include <queue>

class Window;
class Camera;
class UICamera;

class ResourceManager
{
	friend class FrameCommander;
	class PipeLineResources
	{
		struct RootSignatureResources
		{
			std::shared_ptr<RootSignature> pRootSignature;
			std::unordered_map<std::string, DrawableArray> DrawIndexed;
			std::vector<Drawable*> pDrawablesToInitialize;
		};
	public:
		std::shared_ptr<PipelineStateObject> pPipeLineStateObject;
		std::unordered_map<std::string, RootSignatureResources> RootSignatures;
	};
public:
	ResourceManager() = delete;
	ResourceManager(ResourceManager&) = delete;

	Engine_API std::shared_ptr<VertexBuffer> CreateVertexBuffer(Drawable* pDrawable, const void* pData, const unsigned int Stride, unsigned int DataSize, VertexLayout& Lay, unsigned int VertexCount, bool unique = false, unsigned int Slot = 0) noexcept;
	Engine_API std::shared_ptr<IndexBuffer> CreateIndexBuffer(Drawable* pDrawable, std::vector<unsigned int>* Indecies) noexcept;
	Engine_API std::string CreateRootSignature(Drawable* pDrawable, std::string& PSO_Key, RS_Layout& Lay) noexcept;
	Engine_API std::string CreatePSO(PSO_Layout& pLay, VertexLayout& vLay, bool ForUI = false) noexcept;
	Engine_API std::shared_ptr<ConstantBuffer> CreateConstBuffer(Drawable* pDrawable, const void* pData, unsigned int DataSize, UINT RootParam, UINT Range, UINT RangeIndex) noexcept;
	Engine_API std::shared_ptr<Texture2D> CreateTexture2D(Drawable* pDrawable, const std::string& Path, UINT RootParam, UINT Range, UINT RangeIndex, bool OnlyPixelShader = false);
	Engine_API std::shared_ptr<Sampler> CreateDefaultSampler(Drawable* pDrawable, UINT RootParam, UINT Range, UINT RangeIndex) noexcept;
	
	Engine_API ResourceManager(Window* pWindow) noexcept;
	Engine_API void InitializeResources(Window* pWindow);
	Engine_API const DirectX::XMMATRIX* GetProjectionForUI() noexcept;
	Engine_API const DirectX::XMMATRIX* GetPerspectiveProjection() noexcept;
	Engine_API const DirectX::XMMATRIX* GetView() noexcept;
	Engine_API const DirectX::XMMATRIX* GetViewForUI() noexcept;
	Engine_API void SetCamera(Camera* cam) noexcept;
	Engine_API void SetCameraForUI(Camera* cam) noexcept;

	Engine_API ~ResourceManager();

private:
	void InitCamera(std::unordered_map<std::string, PipeLineResources>* Resources);

private:
	GlobalHeap Heap;

	std::unordered_map<std::string, std::shared_ptr<Bindable>> Bindables;
	std::unordered_map<std::string, std::shared_ptr<Resource>> Resources;
	std::unordered_map<std::string, PipeLineResources> PSO_Drawables;
	std::unordered_map<std::string, PipeLineResources> UI_PSO_Drawables;
	
	ScriptManager* pScriptManager;

	// Default cameras.
	Camera* DefaultCamera;
	Camera* DefaultCameraUI;

	// User's cameras.
	Camera* camera = nullptr;
	Camera* cameraUI = nullptr;
};

#endif
