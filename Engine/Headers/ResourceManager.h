#pragma once
#ifndef RESOURCE_MANAGER_HEADER
#define RESOURCE_MANAGER_HEADER
#include "Graphics\Resources\Bindable.h"
#include "Graphics\Resources\BindablesHeader.h"
#include "Graphics\Resources\Drawable.h"
#include <unordered_map>
#include <string>
#include <memory>
#include <queue>


class ResourceManager
{
	friend class FrameCommander;
	class PipeLineResources
	{
		struct RootSignatureResources
		{
			std::shared_ptr<RootSignature> pRootSignature;
			std::vector<Drawable*> pDrawables;
			UINT Count = 0u;
		};
	public:
		std::shared_ptr<PipelineStateObject> pPipeLineStateObject;
		std::unordered_map<std::string, RootSignatureResources> RootSignatures;
	};
public:
	std::shared_ptr<VertexBuffer> CreateVertexBuffer(Drawable* pDrawable, void* pData, const unsigned int Stride, unsigned int DataSize, VertexLayout Lay, unsigned int Slot = 0);
	std::shared_ptr<IndexBuffer> CreateIndexBuffer(Drawable* pDrawable, std::vector<unsigned int> Indecies);
	std::string CreateRootSignature(std::string& PSO_Key, RS_Layout& Lay, Drawable* pDrawable);
	std::string CreatePSO(PSO_Layout& pLay, VertexLayout* vLay);
	std::shared_ptr<ConstantBuffer> CreateConstBuffer(Drawable* pDrawable, void* pData, unsigned int DataSize, UINT RootParam, UINT Range, UINT RangeIndex);
	std::shared_ptr<Texture2D> CreateTexture2D(Drawable* pDrawable, std::string Path, UINT RootParam, UINT Range, UINT RangeIndex, bool OnlyPixelShader = false);
	std::shared_ptr<Sampler> CreateDefaultSampler(Drawable* pDrawable, UINT RootParam, UINT Range, UINT RangeIndex);
	
	Engine_API void InitializeResources(Graphics* pGraphics);

private:
	GlobalHeap Heap;

	std::unordered_map<std::string, std::shared_ptr<Bindable>> Bindables;
	std::unordered_map<std::string, PipeLineResources> Resources;
};

#endif
