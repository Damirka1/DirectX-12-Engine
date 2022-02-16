#include "../../Headers/ResourceManager.h"
#include "../../Headers/Window.h"
#include "../../Headers/DirectXTex.h"
#include "../../Headers/Utility.h"
#include "../../Headers/Scene/Scene.h"

ResourceManager::ResourceManager(Window* pWindow) noexcept
	:
	pGraphics(pWindow->GetGraphics())
{
}

std::shared_ptr<VertexBuffer> ResourceManager::CreateVertexBuffer(std::string* key, const void* pData, unsigned int Stride, unsigned int DataSize, VertexLayout& Lay, unsigned int VertexCount, unsigned int Slot) noexcept
{
	const auto i = Bindables.find(*key);
	if (i == Bindables.end())
	{
		auto bind = std::make_shared<VertexBuffer>(pData, Stride, DataSize, VertexCount, Slot);
		Bindables[*key] = bind;
		return bind;
	}
	return std::static_pointer_cast<VertexBuffer>(i->second);
}

std::shared_ptr<IndexBuffer> ResourceManager::CreateIndexBuffer(std::string* key, std::vector<unsigned int>* Indecies) noexcept
{
	
	const auto i = Bindables.find(*key);
	if (i == Bindables.end())
	{
		auto bind = std::make_shared<IndexBuffer>(std::move(Indecies));
		Bindables[*key] = bind;
		return bind;
	}
	return std::static_pointer_cast<IndexBuffer>(i->second);
}

std::shared_ptr<RootSignature> ResourceManager::CreateRootSignature(std::shared_ptr<DrawableArray> pArray, std::string& PSO_Key, RS_Layout& Lay) noexcept
{
	using namespace std::string_literals;
	const std::string key = typeid(RootSignature).name() + "#"s + Lay.GetCode();
	const auto i = Bindables.find(key);
	
	std::shared_ptr<RootSignature> bind;

	if (i == Bindables.end())
	{
		bind = std::make_shared<RootSignature>(Lay);
		bind->SetKey(key);
		Bindables[key] = bind;
	}
	else
		bind = std::static_pointer_cast<RootSignature>(i->second);

	// If heap is not exists.
	if (!pArray->pHeap)
	{
		// Create heap.
		std::shared_ptr<HeapDescriptorArray> pHeap = std::make_shared<HeapDescriptorArray>();
		// Initialize.
		pHeap->Initialize(Lay);
		// Insert in the beggining of bindables.
		pArray->pHeap = pHeap;
	}
	
	return bind;
}

std::shared_ptr<PipelineStateObject> ResourceManager::CreatePipelineStateObject(PSO_Layout& pLay, VertexLayout& vLay) noexcept
{
	using namespace std::string_literals;
	const std::string key = typeid(PipelineStateObject).name() + "#"s + pLay.GetCode() + "#"s + vLay.GetCode();
	const auto i = Bindables.find(key);
	if (i == Bindables.end())
	{
		auto bind = std::make_shared<PipelineStateObject>(pLay, vLay);
		bind->SetKey(key);
		Bindables[key] = bind;
		return bind;
	}
	return std::static_pointer_cast<PipelineStateObject>(i->second);
}

std::shared_ptr<ConstantBuffer> ResourceManager::CreateConstBuffer(Drawable* pDrawable, const void* pData, unsigned int DataSize, UINT RootParam, UINT Range, UINT RangeIndex) noexcept
{
	auto bind = std::make_shared<ConstantBuffer>(pData, DataSize);
	bind->SetHeapIndex(RootParam, Range, RangeIndex);

	// Increment desc count.
	Heap.Add_CBV_SHR_UAV_Desc(1);

	pDrawable->Array->InitList.push_back(bind);
	return bind;
}

void ResourceManager::InitializeResources(Scene* pScene)
{
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

	for (auto& Array : DrArraysToInit)
	{
		Array.second->pRootSignature->Initialize(pGraphics);
		Array.second->pVertexBuffer->Initialize(pGraphics);
		Array.second->pIndexBuffer->Initialize(pGraphics);

		if (!Array.second->pHeap)
			continue;

		auto Heap = Array.second->pHeap;
		Heap->InitializePointers(pGraphics, pCPUStart, pGPUStart, pCPUStartSamplers, pGPUStartSamplers);
		for (auto& El : Array.second->InitList)
		{
			CD3DX12_CPU_DESCRIPTOR_HANDLE Ptr = Heap->GetCPUHandle(El->Table, El->Range, El->Index);
			El->Initialize(pGraphics, Ptr);
		}
		Array.second->InitList.clear();
		Array.second->pPipelineStateObject->Initialize(pGraphics, Array.second->pRootSignature.get());
	}

	pGraphics->Initialize();

	DrArraysToInit.clear();

	// First initialize RootSignatures and PipeLineStateObjects.
	//for (auto& PSO : pScene->DrawablesMap)
	//{
	//	for (auto& RS : PSO.second.RootSignatures)
	//	{
	//		RS.second.pRootSignature->Initialize(pGraphics);

	//		// Initialize bindables in drawables.
	//		for (auto& Drawable : RS.second.pDrawablesToInitialize)
	//		{
	//			// Initialize common things (vertex, index buffers and so on).
	//			for (auto& Common : Drawable->InitializeCommon_list)
	//				Common->Initialize(pGraphics);

	//			Drawable->InitializeCommon_list.clear();

	//			// Now add drawable to array. The same drawable will be bind vertex and index buffers only once.
	//			std::string key = Drawable->pVertexBuffer->GetKey() + Drawable->pIndexBuffer->GetKey();
	//			DrawableArray* pArray = &RS.second.DrawIndexed[key];
	//			pArray->AddDrawable(Drawable);
	//			pArray->SetVertexAndIndexBuffers(Drawable->pVertexBuffer, Drawable->pIndexBuffer);

	//			if (Drawable->DescHeapIndex == -1)
	//				continue;

	//			// Set pointers to descriptor array.
	//			auto HeapDesc = std::static_pointer_cast<HeapDescriptorArray>(Drawable->Bindables[Drawable->DescHeapIndex]);

	//			HeapDesc->InitializePointers(pGraphics, pCPUStart,
	//				pGPUStart, pCPUStartSamplers, pGPUStartSamplers);

	//			// Initialize heap elements in drawables.
	//			for (auto& HeapEl : Drawable->InitializeHeap_list)
	//			{
	//				CD3DX12_CPU_DESCRIPTOR_HANDLE Ptr = HeapDesc->GetCPUHandle(HeapEl->Table, HeapEl->Range, HeapEl->Index);
	//				HeapEl->Initialize(pGraphics, Ptr);
	//			}
	//			Drawable->InitializeHeap_list.clear();
	//		}

	//		RS.second.pDrawablesToInitialize.clear();
	//	}
	//	PSO.second.pPipeLineStateObject->Initialize(pGraphics, PSO.second.RootSignatures.begin()->second.pRootSignature.get());
	//}

	//pGraphics->Initialize();

	pScene->InitCamera();
}

ResourceManager::~ResourceManager()
{
}

Engine_API void ResourceManager::CreateVIBuffers(Drawable* pDrawable, std::vector<unsigned int>* Indecies, const void* pData, const unsigned int Stride, unsigned int DataSize, VertexLayout& Lay, unsigned int VertexCount, unsigned int Slot)
{
	using namespace std::string_literals;
	std::string VBkey = typeid(VertexBuffer).name() + "#"s + std::to_string(Stride) + "#"s + std::to_string(DataSize) + "#"s + std::to_string(Slot) + "{" + Lay.GetCode() + "}" + "#"s + "{" + "#"s + std::to_string(((float*)pData)[0]) + "#"s
		+ std::to_string(((float*)pData)[1]) + "#"s + std::to_string(((float*)pData)[3]) + "}";

	std::string IBkey = typeid(IndexBuffer).name() + "#"s + std::to_string(Indecies->size()) + "{" + std::to_string((*Indecies)[0]) + "#"s + std::to_string((*Indecies)[1]) + "#"s + std::to_string((*Indecies)[2]) + "}";

	std::string key = VBkey + IBkey;

	if (Drawables.find(key) == Drawables.end())
	{
		std::shared_ptr<VertexBuffer> VB = CreateVertexBuffer(&VBkey, pData, Stride, DataSize, Lay, VertexCount, Slot);
		std::shared_ptr<IndexBuffer> IB = CreateIndexBuffer(&IBkey, std::move(Indecies));
		pDrawable->Array = std::make_shared<DrawableArray>(VB, IB, key);
		pDrawable->Array->AddDrawable(pDrawable);
		DrArraysToInit[key] = pDrawable->Array;
	}
}

Engine_API void ResourceManager::CreatePSRS(Drawable* pDrawable, PSO_Layout& PL, RS_Layout& RL, VertexLayout& VL)
{
	std::shared_ptr<PipelineStateObject> PSO = CreatePipelineStateObject(PL, VL);
	std::shared_ptr<RootSignature> RS = CreateRootSignature(pDrawable->Array, PSO->GetKey(), RL);
	pDrawable->Array->SetPSRS(PSO, RS);
}

std::shared_ptr<Texture2D> ResourceManager::CreateTexture2D(Drawable* pDrawable, const std::string& Path, UINT RootParam, UINT Range, UINT RangeIndex, bool OnlyPixelShader)
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
		bind->SetHeapIndex(RootParam, Range, RangeIndex);
		Resources[Path] = bind;

		// Increment desc count.
		Heap.Add_CBV_SHR_UAV_Desc(1);

		pDrawable->Array->InitList.push_back(bind);
		return bind;
	}

	// Add to initialize list for setting descriptors.
	auto bind = std::static_pointer_cast<Texture2D>(i->second);
	// Increment desc count.
	Heap.Add_CBV_SHR_UAV_Desc(1);
	pDrawable->Array->InitList.push_back(bind);

	return bind;
}

std::shared_ptr<Sampler> ResourceManager::CreateDefaultSampler(Drawable* pDrawable, UINT RootParam, UINT Range, UINT RangeIndex) noexcept
{
	std::string key = "DEFAULT SAMPLER";
	const auto i = Resources.find(key);

	if (i == Resources.end())
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

		auto bind = std::make_shared<Sampler>(&sampler);
		bind->SetHeapIndex(RootParam, Range, RangeIndex);

		// Increment desc count.
		Heap.Add_Samplers_Desc(1);

		pDrawable->Array->InitList.push_back(bind);
		Resources[key] = bind;
		return bind;
	}

	auto bind = std::static_pointer_cast<Sampler>(i->second);
	// Increment desc count.
	Heap.Add_Samplers_Desc(1);
	pDrawable->Array->InitList.push_back(bind);

	return bind;
}

