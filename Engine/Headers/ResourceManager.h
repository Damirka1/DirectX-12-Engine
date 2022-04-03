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
#include "Graphics/Resources/Heap.h"
#include "Graphics/Resources/Buffers/Buffers.h"
#include "Graphics/Resources/Buffers/Samplers.h"
#include "Graphics/Resources/Buffers/Textures.h"

class Window;
class Scene;

class ResourceManager
{
	friend class FrameCommander;
public:
	Engine_API ResourceManager(Window* pWindow) noexcept;
	ResourceManager(ResourceManager&) = delete;

	Engine_API std::shared_ptr<VertexBuffer> CreateVertexBuffer(const void* pData, const unsigned int Stride, unsigned int DataSize, VertexLayout& Lay, unsigned int VertexCount, unsigned int Slot = 0) noexcept;
	Engine_API std::shared_ptr<IndexBuffer> CreateIndexBuffer(std::vector<unsigned int>* Indecies) noexcept;
	Engine_API std::shared_ptr<RootSignature> CreateRootSignature(RS_Layout& Lay) noexcept;
	Engine_API std::shared_ptr<PipelineStateObject> CreatePipelineStateObject(PSO_Layout& pLay, VertexLayout& vLay) noexcept;
	Engine_API std::shared_ptr<ConstantBuffer> CreateConstBuffer(const void* pData, unsigned int DataSize, UINT Index) noexcept;
	Engine_API std::shared_ptr<Texture2D> CreateTexture2D(const std::string& Path, UINT Index, bool OnlyPixelShader = false);
	Engine_API std::shared_ptr<Sampler> CreateDefaultSampler(UINT Index) noexcept;
	
	Engine_API void InitializeResources(Scene* pScene);
	Engine_API ~ResourceManager();

private:
	Graphics* pGraphics;
	GlobalHeap Heap;

	std::unordered_map<std::string, std::shared_ptr<PipelineStateObject>> PipelineStateObjects;
	std::unordered_map<std::string, std::shared_ptr<RootSignature>> RootSignatures;
	std::unordered_map<std::string, std::shared_ptr<VertexBuffer>> VertexBuffers;
	std::unordered_map<std::string, std::shared_ptr<IndexBuffer>> IndexBuffers;
	std::unordered_map<std::string, std::shared_ptr<Resource>> Resources;
	std::unordered_map<std::string, std::shared_ptr<Sampler>> Samplers;
	std::vector<std::shared_ptr<ConstantBuffer>> ConstBuffers;
};

#endif
