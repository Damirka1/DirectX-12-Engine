#include "../../Headers/ResourceManager.h"
#include "../../Headers/Window.h"
#include "../../Headers/DirectXTex.h"
#include "../../Headers/Utility.h"
#include "../../Headers/Scene/Scene.h"

#include "../../Headers/Graphics/Resources/Heap.h"
#include "../../Headers/Graphics/Resources/ResourceHeader.h"
#include "../../Headers/Graphics/Resources/DrawableMesh.h"
#include "../../Headers/Graphics/Resources/DrawableMeshMaterial.h"

ResourceManager::ResourceManager(Window* pWindow) noexcept
	:
	pGraphics(pWindow->GetGraphics())
{
}

std::shared_ptr<VertexBuffer> ResourceManager::CreateVertexBuffer(const void* pData, unsigned int Stride, unsigned int DataSize, VertexLayout& Lay, unsigned int VertexCount, unsigned int Slot) noexcept
{
	using namespace std::string_literals;
	std::string key = typeid(VertexBuffer).name() + "#"s + std::to_string(Stride) + "#"s + std::to_string(DataSize) + "#"s + std::to_string(Slot) + "{" + Lay.GetCode() + "}" + "#"s + "{" + "#"s + std::to_string(((float*)pData)[0]) + "#"s
		+ std::to_string(((float*)pData)[1]) + "#"s + std::to_string(((float*)pData)[3]) + "}";

	const auto i = VertexBuffers.find(key);
	if (i == VertexBuffers.end())
	{
		auto res = std::make_shared<VertexBuffer>(pData, Stride, DataSize, VertexCount, Slot);
		VertexBuffers[key] = res;
		return res;
	}
	return i->second;
}

std::shared_ptr<IndexBuffer> ResourceManager::CreateIndexBuffer(std::vector<unsigned int>* Indecies) noexcept
{
	using namespace std::string_literals;
	std::string key = typeid(IndexBuffer).name() + "#"s + std::to_string(Indecies->size()) + "{" + std::to_string((*Indecies)[0]) + "#"s + std::to_string((*Indecies)[1]) + "#"s + std::to_string((*Indecies)[2]) + "}";
	const auto i = IndexBuffers.find(key);
	if (i == IndexBuffers.end())
	{
		auto res = std::make_shared<IndexBuffer>(std::move(Indecies));
		IndexBuffers[key] = res;
		return res;
	}
	return i->second;
}

std::shared_ptr<RootSignature> ResourceManager::CreateRootSignature(RS_Layout& Lay) noexcept
{
	using namespace std::string_literals;
	const std::string key = typeid(RootSignature).name() + "#"s + Lay.GetCode();
	const auto i = RootSignatures.find(key);
	
	if (i == RootSignatures.end())
	{
		auto res = std::make_shared<RootSignature>(Lay);
		res->SetKey(key);
		RootSignatures[key] = res;
		return res;
	}
	return i->second;
}

std::shared_ptr<PipelineStateObject> ResourceManager::CreatePipelineStateObject(PSO_Layout& pLay, VertexLayout& vLay) noexcept
{
	using namespace std::string_literals;
	const std::string key = typeid(PipelineStateObject).name() + "#"s + pLay.GetCode() + "#"s + vLay.GetCode();
	const auto i = PipelineStateObjects.find(key);
	if (i == PipelineStateObjects.end())
	{
		auto res = std::make_shared<PipelineStateObject>(pLay, vLay);
		res->SetKey(key);
		PipelineStateObjects[key] = res;
		return res;
	}
	return i->second;
}

std::shared_ptr<ConstantBuffer> ResourceManager::CreateConstBuffer(const void* pData, unsigned int DataSize, UINT Index) noexcept
{
	auto res = std::make_shared<ConstantBuffer>(pData, DataSize);
	res->SetHeapIndex(Index);

	// Increment desc count.
	Heap.Add_CBV_SHR_UAV_Desc(1);

	ConstBuffers.push_back(res);

	return res;
}

// How it's work
/*
	if we want to upload something on gpu heap on running engine,
	we clear all heap state to upload new big one with new preallocated heaps


*/

void ResourceManager::InitializeResources(Scene* pScene)
{
	if (!Heap.IsNeedUpdate())
	{
		pGraphics->Initialize();
		return;
	}

	// Clear Heap.
	Heap.Clear(pGraphics);

	// Initialize Heap.
	Heap.Initialize(pGraphics);

	CD3DX12_CPU_DESCRIPTOR_HANDLE pCPUStart;
	CD3DX12_GPU_DESCRIPTOR_HANDLE pGPUStart;
	if (Heap.isHeapForCbvInitialized())
	{
		pCPUStart = CD3DX12_CPU_DESCRIPTOR_HANDLE(Heap.GetCPUStartPtr());
		pGPUStart = CD3DX12_GPU_DESCRIPTOR_HANDLE(Heap.GetGPUStartPtr());
	}

	CD3DX12_CPU_DESCRIPTOR_HANDLE pCPUStartSamplers;
	CD3DX12_GPU_DESCRIPTOR_HANDLE pGPUStartSamplers;
	if (Heap.isHeapForSamplersInitialized())
	{
		pCPUStartSamplers = CD3DX12_CPU_DESCRIPTOR_HANDLE(Heap.GetCPUStartPtrForSAMPLERS());
		pGPUStartSamplers = CD3DX12_GPU_DESCRIPTOR_HANDLE(Heap.GetGPUStartPtrForSAMPLERS());
	}

	for (auto& rs : RootSignatures)
		rs.second->Initialize(pGraphics);
	for (auto& pso : PipelineStateObjects)
		pso.second->Initialize(pGraphics, RootSignatures.begin()->second.get());
	for (auto& vb : VertexBuffers)
		vb.second->Initialize(pGraphics);
	for (auto& ib : IndexBuffers)
		ib.second->Initialize(pGraphics);

	auto pDevice = pGraphics->GetDevice();
	UINT IncrementSizeCbv = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	UINT IncrementSizeSmp = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

	for (auto& rs : Resources)
	{
		rs.second->Initialize(pGraphics, pCPUStart);
		rs.second->pGpuHandle = pGPUStart;
		pCPUStart.Offset(IncrementSizeCbv);
		pGPUStart.Offset(IncrementSizeCbv);
	}

	for (auto& cb : ConstBuffers)
	{
		cb->Initialize(pGraphics, pCPUStart);
		cb->pGpuHandle = pGPUStart;
		pCPUStart.Offset(IncrementSizeCbv);
		pGPUStart.Offset(IncrementSizeCbv);
	}

	for (auto& s : Samplers)
	{
		s.second->Initialize(pGraphics, pCPUStartSamplers);
		s.second->pGpuHandle = pGPUStartSamplers;
		pCPUStartSamplers.Offset(IncrementSizeSmp);
		pGPUStartSamplers.Offset(IncrementSizeSmp);
	}

	//for (auto& Array : DrArraysToInit)
	//{
	//	Array.second->pRootSignature->Initialize(pGraphics);
	//	Array.second->pVertexBuffer->Initialize(pGraphics);
	//	Array.second->pIndexBuffer->Initialize(pGraphics);

	//	if (!Array.second->pHeap)
	//		continue;

	//	auto Heap = Array.second->pHeap;
	//	Heap->InitializePointers(pGraphics, pCPUStart, pGPUStart, pCPUStartSamplers, pGPUStartSamplers);
	//	for (auto& El : Array.second->InitList)
	//	{
	//		CD3DX12_CPU_DESCRIPTOR_HANDLE Ptr = Heap->GetCPUHandle(El->Table, El->Range, El->Index);
	//		El->Initialize(pGraphics, Ptr);
	//	}
	//	//Array.second->InitList.clear();
	//	Array.second->pPipelineStateObject->Initialize(pGraphics, Array.second->pRootSignature.get());
	//	Array.second->SetReady(true);
	//}

	pGraphics->Initialize();

	//pScene->InitCamera();
}

ResourceManager::~ResourceManager()
{
}

std::shared_ptr<Texture2D> ResourceManager::CreateTexture2D(const std::string& Path, UINT Index, bool OnlyPixelShader)
{
	const auto i = Resources.find(Path);
	if (i == Resources.end())
	{
		// Load image from file.
		std::unique_ptr<DirectX::ScratchImage> img = std::make_unique<DirectX::ScratchImage>();
		if (DirectX::LoadFromWICFile(StringToWString(Path).c_str(), DirectX::WIC_FLAGS_NONE, nullptr, *img) != S_OK)
			throw std::exception("Can't load texture");

		// Get meta data from image.
		auto& MetaData = img->GetMetadata();

		// Describe and create a Texture2D.
		D3D12_RESOURCE_DESC textureDesc = {};
		textureDesc.MipLevels = 1;
		textureDesc.Format = MetaData.format; //pGraphics->GetRTVFormat();
		textureDesc.Width = static_cast<UINT>(MetaData.width);
		textureDesc.Height = static_cast<UINT>(MetaData.height);
		textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		textureDesc.DepthOrArraySize = 1;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

		// Create texture pointer.
		auto bind = std::make_shared<Texture2D>(std::move(img), &textureDesc, OnlyPixelShader);
		bind->SetHeapIndex(Index);
		Resources[Path] = bind;

		// Increment desc count.
		Heap.Add_CBV_SHR_UAV_Desc(1);

		return bind;
	}
	// Add to initialize list for setting descriptors.
	return std::static_pointer_cast<Texture2D>(i->second);
}

std::shared_ptr<Sampler> ResourceManager::CreateDefaultSampler(UINT Index) noexcept
{
	std::string key = "DEFAULT SAMPLER";
	const auto i = Samplers.find(key);

	if (i == Samplers.end())
	{
		D3D12_SAMPLER_DESC sampler;
		sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
		sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		sampler.MipLODBias = 0;
		sampler.MaxAnisotropy = 0;
		sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		//sampler.BorderColor[0] = 1.0f;
		//sampler.BorderColor[1] = 1.0f;
		//sampler.BorderColor[2] = 1.0f;
		//sampler.BorderColor[3] = 1.0f;
		sampler.MinLOD = 0.0f;
		sampler.MaxLOD = D3D12_FLOAT32_MAX;

		auto res = std::make_shared<Sampler>(&sampler);
		res->SetHeapIndex(Index);

		// Increment desc count.
		Heap.Add_Samplers_Desc(1);

		Samplers[key] = res;
		return res;
	}

	return std::static_pointer_cast<Sampler>(i->second);
}

