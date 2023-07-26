#pragma once
#ifndef RESOURCE_MANAGER_HEADER
#define RESOURCE_MANAGER_HEADER
#include "Header.h"
#include <unordered_map>
#include <string>
#include <memory>
#include <queue>

#include "Graphics.h"
#include "Graphics/Resources/PipeLineState.h"
#include "Graphics/Resources/RootSignature.h"
#include "Graphics/Resources/VertexLayout.h"
#include "Graphics/Resources/Resource.h"
#include "Graphics/Resources/Buffers/Buffers.h"
#include "Graphics/Resources/Buffers/Samplers.h"
#include "Graphics/Resources/Buffers/Textures.h"

#include "Graphics/Resources/Heap/DescriptorsGlobalHeap.h"
#include "Graphics/RayTracing/RTXResources.h"

class Graphics;
class Scene;

class ResourceManager
{
	friend class FrameCommander;
public:
	Engine_API ResourceManager(Graphics* pGraphics) noexcept;
	ResourceManager(ResourceManager&) = delete;

	Engine_API std::shared_ptr<VertexBuffer> CreateVertexBuffer(const void* pData, const unsigned int Stride, unsigned int DataSize, VertexLayout& Lay, unsigned int VertexCount, unsigned int Slot = 0) noexcept;
	Engine_API std::shared_ptr<IndexBuffer> CreateIndexBuffer(std::vector<unsigned int>* Indecies) noexcept;
	Engine_API std::shared_ptr<RootSignature> CreateRootSignature(RS_Layout& Lay) noexcept;
	Engine_API std::shared_ptr<PipelineStateObject> CreatePipelineStateObject(PSO_Layout& pLay, VertexLayout& vLay, std::shared_ptr<RootSignature> pRootSignature) noexcept;
	Engine_API std::shared_ptr<ConstantBuffer> CreateConstBuffer(const void* pData, unsigned int DataSize, UINT Index) noexcept;
	Engine_API std::shared_ptr<Texture2D> CreateTexture2D(const std::string& Path, UINT Index, bool OnlyPixelShader = false);
	Engine_API std::shared_ptr<Sampler> CreateDefaultSampler(UINT Index) noexcept;

	Engine_API void Update();

	Engine_API void BindHeap();
	
	Engine_API void InitializeResources();
	Engine_API ~ResourceManager();

private:
	Graphics* pGraphics;
	DescriptorsGlobalHeap Heap;

	// srv uav
	std::unordered_map<std::string, std::shared_ptr<Resource>> HeapResources;
	// vb ib rs pso
	std::unordered_map<std::string, std::shared_ptr<Resource>> BaseResources;
	std::unordered_map<std::string, std::shared_ptr<Sampler>> Samplers;
	std::vector<std::shared_ptr<ConstantBuffer>> ConstBuffers;
	std::vector<std::shared_ptr<Resource>> ToInit;
};

#endif
