#include "..\..\Headers\ResourceManager.h"
#include "..\..\Headers\Window.h"
#include "..\..\Headers\DirectXTex.h"
#include "..\..\Headers\Utility.h"
#include "..\..\Headers\Input\Camera.h"

std::shared_ptr<VertexBuffer> ResourceManager::CreateVertexBuffer(Drawable* pDrawable, const void* pData, unsigned int Stride, unsigned int DataSize, VertexLayout& Lay, unsigned int VertexCount, bool unique, unsigned int Slot) noexcept
{
	using namespace std::string_literals;
	std::string key = typeid(VertexBuffer).name() + "#"s + std::to_string(Stride) + "#"s + std::to_string(DataSize) + "#"s + std::to_string(Slot) + "{" + Lay.GetCode() + "}";
	if (unique)
	{
		static UINT Index;
		key += std::to_string(Index++);
	}
	const auto i = Bindables.find(key);
	if (i == Bindables.end())
	{
		auto bind = std::make_shared<VertexBuffer>(pData, Stride, DataSize, VertexCount, Slot);
		Bindables[key] = bind;
		pDrawable->InitializeCommon_list.push_back(bind);
		return bind;
	}
	return std::static_pointer_cast<VertexBuffer>(i->second);
}

std::shared_ptr<IndexBuffer> ResourceManager::CreateIndexBuffer(Drawable* pDrawable, std::vector<unsigned int>* Indecies) noexcept
{
	using namespace std::string_literals;
	const std::string key = typeid(IndexBuffer).name() + "#"s + std::to_string(Indecies->size()) + "{" + std::to_string((*Indecies)[0]) + "}";
	const auto i = Bindables.find(key);
	if (i == Bindables.end())
	{
		auto bind = std::make_shared<IndexBuffer>( std::move(Indecies));
		Bindables[key] = bind;
		pDrawable->InitializeCommon_list.push_back(bind);
		return bind;
	}
	return std::static_pointer_cast<IndexBuffer>(i->second);
}

std::string ResourceManager::CreateRootSignature(Drawable* pDrawable, std::string& PSO_Key, RS_Layout& Lay) noexcept
{
	using namespace std::string_literals;
	const std::string key = typeid(RootSignature).name() + "#"s + Lay.GetCode();
	const auto i = Bindables.find(key);
	
	// Set keys in drawable.
	pDrawable->PSO_Key = PSO_Key;
	pDrawable->RS_Key = key;

	std::shared_ptr<RootSignature> bind;

	if (i == Bindables.end())
	{
		bind = std::make_shared<RootSignature>(Lay);
		Bindables[key] = bind;
	}
	else
		bind = std::static_pointer_cast<RootSignature>(i->second);

	if (pDrawable->UI_element)
	{
		UI_PSO_Drawables[PSO_Key].RootSignatures[key].pDrawablesToInitialize.push_back(pDrawable);
		auto& el = UI_PSO_Drawables[PSO_Key].RootSignatures[key];
		el.pRootSignature = bind;
	}
	else
	{
		PSO_Drawables[PSO_Key].RootSignatures[key].pDrawablesToInitialize.push_back(pDrawable);
		auto& el = PSO_Drawables[PSO_Key].RootSignatures[key];
		el.pRootSignature = bind;
	}

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

std::string ResourceManager::CreatePSO(PSO_Layout& pLay, VertexLayout& vLay, bool ForUI) noexcept
{
	using namespace std::string_literals;
	const std::string key = typeid(PipelineStateObject).name() + "#"s + pLay.GetCode() + "#"s + vLay.GetCode();
	const auto i = Bindables.find(key);
	if (i == Bindables.end())
	{
		auto bind = std::make_shared<PipelineStateObject>(pLay, vLay);
		Bindables[key] = bind;
		if (ForUI)
			UI_PSO_Drawables[key].pPipeLineStateObject = bind;
		else
			PSO_Drawables[key].pPipeLineStateObject = bind;
		return key;
	}
	return key;
}

std::shared_ptr<ConstantBuffer> ResourceManager::CreateConstBuffer(Drawable* pDrawable, const void* pData, unsigned int DataSize, UINT RootParam, UINT Range, UINT RangeIndex) noexcept
{
	auto bind = std::make_shared<ConstantBuffer>(pData, DataSize);
	bind->SetHeapIndex(RootParam, Range, RangeIndex);

	// Increment desc count.
	Heap.Add_CBV_SHR_UAV_Desc(1);

	pDrawable->InitializeHeap_list.push_back(bind);
	return bind;
}

void ResourceManager::InitializeResources(Window* pWindow)
{
	pScriptManager = new ScriptManager(pWindow);

	Graphics* pGraphics = pWindow->GetGraphics();
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

	auto Init = [&](std::unordered_map<std::string, PipeLineResources>& Resources)
	{
		// First initialize RootSignatures and PipeLineStateObjects.
		for (auto& PSO : Resources)
		{
			for (auto& RS : PSO.second.RootSignatures)
			{
				RS.second.pRootSignature->Initialize(pGraphics);

				// Initialize bindables in drawables.
				for (auto& Drawable : RS.second.pDrawablesToInitialize)
				{
					// Initialize common things (vertex, index buffers and so on).
					for (auto& Common : Drawable->InitializeCommon_list)
						Common->Initialize(pGraphics);

					Drawable->InitializeCommon_list.clear();

					// Now add drawable to array. The same drawable will be bind vertex and index buffers only once.
					std::string key = Drawable->pVertexBuffer->GetKey() + Drawable->pIndexBuffer->GetKey();
					DrawableArray* pArray = &RS.second.DrawIndexed[key];
					pArray->AddDrawable(Drawable);
					pArray->SetVertexAndIndexBuffers(Drawable->pVertexBuffer, Drawable->pIndexBuffer);

					// Add drawable to script manager.
					pScriptManager->AddDrawable(Drawable);

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

				RS.second.pDrawablesToInitialize.clear();
			}
			PSO.second.pPipeLineStateObject->Initialize(pGraphics, PSO.second.RootSignatures.begin()->second.pRootSignature.get());
		}
	};

	Init(PSO_Drawables);

	Init(UI_PSO_Drawables);

	pGraphics->Initialize();

	pScriptManager->StartListen();
}

ResourceManager::ResourceManager(Window* pWindow) noexcept
{
	auto res = pWindow->GetGraphicsResolution();

	DefaultCamera = new Camera(res);
	DefaultCameraUI = new Camera(res, Camera::ProjectionType::Orthographic);
}

const DirectX::XMMATRIX* ResourceManager::GetProjectionForUI() noexcept
{
	if (cameraUI)
		return &cameraUI->GetProjection();
	return &DefaultCameraUI->GetProjection();
}

const DirectX::XMMATRIX* ResourceManager::GetPerspectiveProjection() noexcept
{
	if (camera)
		return &camera->GetProjection();
	return &DefaultCamera->GetProjection();
}

const DirectX::XMMATRIX* ResourceManager::GetView() noexcept
{
	if (camera)
		return &camera->GetView();
	return &DefaultCamera->GetView();
}

const DirectX::XMMATRIX* ResourceManager::GetViewForUI() noexcept
{
	if (cameraUI)
		return &cameraUI->GetView();
	return &DefaultCameraUI->GetView();
}

void ResourceManager::SetCamera(Camera* cam) noexcept
{
	if (!cam)
		return;

	this->camera = cam;
	InitCamera(&PSO_Drawables);
}

void ResourceManager::SetCameraForUI(Camera* cam) noexcept
{
	if (!cam)
		return;

	this->cameraUI = cam;
	InitCamera(&UI_PSO_Drawables);
}

ResourceManager::~ResourceManager()
{
	delete DefaultCamera;
	delete DefaultCameraUI;
	delete pScriptManager;
}

void ResourceManager::InitCamera(std::unordered_map<std::string, PipeLineResources>* Resources)
{
	for (auto& PSO : *Resources)
		for (auto& RS : PSO.second.RootSignatures)
			for (auto& Drawable : RS.second.DrawIndexed)
				Drawable.second.InitCamera(this);
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

		pDrawable->InitializeHeap_list.push_back(bind);
		return bind;
	}

	// Add to initialize list for setting descriptors.
	auto bind = std::static_pointer_cast<Texture2D>(i->second);
	// Increment desc count.
	Heap.Add_CBV_SHR_UAV_Desc(1);
	pDrawable->InitializeHeap_list.push_back(bind);

	return bind;
}

std::shared_ptr<Sampler> ResourceManager::CreateDefaultSampler(Drawable* pDrawable, UINT RootParam, UINT Range, UINT RangeIndex) noexcept
{
	//static int it;
	std::string key = "DEFAULT SAMPLER"; //+ std::to_string(it++);
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

		pDrawable->InitializeHeap_list.push_back(bind);
		Resources[key] = bind;
		return bind;
	}

	auto bind = std::static_pointer_cast<Sampler>(i->second);
	// Increment desc count.
	Heap.Add_Samplers_Desc(1);
	pDrawable->InitializeHeap_list.push_back(bind);

	return bind;
}

