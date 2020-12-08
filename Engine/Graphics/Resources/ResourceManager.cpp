#include "..\..\Headers\ResourceManager.h"
#include "..\..\Headers\DirectXTex.h"
#include "..\..\Headers\Utility.h"

std::shared_ptr<VertexBuffer> ResourceManager::CreateVertexBuffer(Drawable* pDrawable, void* pData, unsigned int Stride, unsigned int DataSize, VertexLayout Lay, unsigned int Slot)
{
	using namespace std::string_literals;
	const std::string key = typeid(VertexBuffer).name() + "#"s + std::to_string(Stride) + "#"s + std::to_string(DataSize) + "#"s + std::to_string(Slot) + "{" + Lay.GetCode() + "}";
	const auto i = Bindables.find(key);
	if (i == Bindables.end())
	{
		auto bind = std::make_shared<VertexBuffer>(pData, Stride, DataSize, Slot);
		Bindables[key] = bind;
		pDrawable->InitializeCommon_list.push_back(bind);
		return bind;
	}
	else
	{
		return std::static_pointer_cast<VertexBuffer>(i->second);
	}
}

std::shared_ptr<IndexBuffer> ResourceManager::CreateIndexBuffer(Drawable* pDrawable, std::vector<unsigned int> Indecies)
{
	using namespace std::string_literals;
	const std::string key = typeid(IndexBuffer).name() + "#"s + std::to_string(Indecies.size()) + "{" + std::to_string(Indecies[0]) + "}";
	const auto i = Bindables.find(key);
	if (i == Bindables.end())
	{
		auto bind = std::make_shared<IndexBuffer>( std::move(Indecies));
		Bindables[key] = bind;
		pDrawable->InitializeCommon_list.push_back(bind);
		return bind;
	}
	else
	{
		return std::static_pointer_cast<IndexBuffer>(i->second);
	}
}

std::string ResourceManager::CreateRootSignature(std::string& PSO_Key, RS_Layout& Lay, Drawable* pDrawable)
{
	using namespace std::string_literals;
	const std::string key = typeid(RootSignature).name() + "#"s + Lay.GetCode();
	const auto i = Bindables.find(key);
	if (i == Bindables.end())
	{
		auto bind = std::make_shared<RootSignature>(Lay);
		Bindables[key] = bind;
		auto& el = Resources[PSO_Key].RootSignatures[key];
		el.pRootSignature = bind;
		el.pDrawables.push_back(pDrawable);
	}
	else
	{
		Resources[PSO_Key].RootSignatures[key].pDrawables.push_back(pDrawable);
	}

	Resources[PSO_Key].RootSignatures[key].Count++;

	// If heap is not exists.
	if (pDrawable->DescHeapIndex == -1)
	{
		// Create heap.
		std::shared_ptr<HeapDescriptorArray> pHeap = std::make_shared<HeapDescriptorArray>();
		// Initialize.
		pHeap->Initialize(Lay);
		// Insert in the beggining of bindables.
		pDrawable->Bindables.insert(pDrawable->Bindables.begin(), std::move(pHeap));
		pDrawable->DescHeapIndex = 0;
	}
	
	
	return key;
}

std::string ResourceManager::CreatePSO(PSO_Layout& pLay, VertexLayout* vLay)
{
	using namespace std::string_literals;
	const std::string key = typeid(PipelineStateObject).name() + "#"s + pLay.GetCode() + "#"s + vLay->GetCode();
	const auto i = Bindables.find(key);
	if (i == Bindables.end())
	{
		auto bind = std::make_shared<PipelineStateObject>(pLay, vLay);
		Bindables[key] = bind;
		Resources[key].pPipeLineStateObject = bind;
		return key;
	}
	else
	{
		return key;
	}
}

std::shared_ptr<ConstantBuffer> ResourceManager::CreateConstBuffer(Drawable* pDrawable, void* pData, unsigned int DataSize, UINT RootParam, UINT Range, UINT RangeIndex)
{
	auto bind = std::make_shared<ConstantBuffer>(pData, DataSize);
	bind->SetHeapIndex(RootParam, Range, RangeIndex);

	// Increment desc count.
	Heap.Add_CBV_SHR_UAV_Desc(1);

	pDrawable->InitializeHeap_list.push_back(bind);
	return bind;
}

void ResourceManager::InitializeResources(Graphics* pGraphics)
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


	// First initialize RootSignatures and PipeLineStateObjects.
	for (auto begin = Resources.begin(), end = Resources.end(); begin != end; ++begin)
	{
		for (auto beginNext = begin->second.RootSignatures.begin(), endNext = begin->second.RootSignatures.end(); beginNext != endNext; ++beginNext)
		{
			beginNext->second.pRootSignature->Initialize(pGraphics);

			// Initialize bindables in drawables.
			for (auto& Drawable : beginNext->second.pDrawables)
			{
				// Initialize common things (vertex, index buffers and so on).
				for (auto& Common : Drawable->InitializeCommon_list)
				{
					Common->Initialize(pGraphics);
				}
				Drawable->InitializeCommon_list.clear();
				

				if (Drawable->DescHeapIndex == -1)
					continue;

				// Set pointers to descriptor array.
				auto HeapDesc = std::static_pointer_cast<HeapDescriptorArray>(Drawable->Bindables[Drawable->DescHeapIndex]);

				HeapDesc->InitializePointers(pGraphics, pCPUStart,
					pGPUStart, pCPUStartSamplers, pGPUStartSamplers);

				// Initialize heap elements in drawables.
				for (auto& HeapEl : Drawable->InitializeHeap_list)
				{
					CD3DX12_CPU_DESCRIPTOR_HANDLE Ptr = HeapDesc->GetCPUHandle(HeapEl->Table, HeapEl->Range, HeapEl->Index);
					HeapEl->Initialize(pGraphics, Ptr);
				}
				Drawable->InitializeHeap_list.clear();
			}

		}
		begin->second.pPipeLineStateObject->Initialize(pGraphics, begin->second.RootSignatures.begin()->second.pRootSignature.get());
	}

	pGraphics->Initialize();
}

std::shared_ptr<Texture2D> ResourceManager::CreateTexture2D(Drawable* pDrawable, std::string Path, UINT RootParam, UINT Range, UINT RangeIndex, bool OnlyPixelShader)
{
	const auto i = Bindables.find(Path);
	if (i == Bindables.end())
	{
		// Load image from file.
		std::unique_ptr<DirectX::ScratchImage> img = std::make_unique<DirectX::ScratchImage>();
		if (DirectX::LoadFromWICFile(StringToWString(Path).c_str(), DirectX::WIC_FLAGS_NONE, nullptr, *img) != S_OK)
		{
			throw std::exception("Can't load texture");
		}
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
		Bindables[Path] = bind;

		// Increment desc count.
		Heap.Add_CBV_SHR_UAV_Desc(1);

		pDrawable->InitializeHeap_list.push_back(bind);
		return bind;
	}
	else
	{
		return std::static_pointer_cast<Texture2D>(i->second);
	}
}

std::shared_ptr<Sampler> ResourceManager::CreateDefaultSampler(Drawable* pDrawable, UINT RootParam, UINT Range, UINT RangeIndex)
{
	const auto i = Bindables.find("DEFAULT_SAMPLER");

	if (i == Bindables.end())
	{
		D3D12_SAMPLER_DESC sampler;
		sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
		sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		sampler.MipLODBias = 0;
		sampler.MaxAnisotropy = 0;
		sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		sampler.MinLOD = 0.0f;
		sampler.MaxLOD = D3D12_FLOAT32_MAX;

		auto bind = std::make_shared<Sampler>(&sampler);
		bind->SetHeapIndex(RootParam, Range, RangeIndex);

		// Increment desc count.
		Heap.Add_Samplers_Desc(1);

		pDrawable->InitializeHeap_list.push_back(bind);
		return bind;
	}
	else
	{
		return std::static_pointer_cast<Sampler>(i->second);
	}
}

