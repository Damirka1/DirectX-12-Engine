#include "../../Headers/ResourceManager.h"
#include "../../Headers/Window.h"
#include "../../Headers/DirectXTex.h"
#include "../../Headers/Utility.h"
#include "../../Headers/Scene/Scene.h"

#include "../../Headers/Graphics/Resources/ResourceHeader.h"
#include "../../Headers/Graphics/Resources/DrawableMesh.h"
#include "../../Headers/Graphics/Resources/DrawableMeshMaterial.h"


ResourceManager::ResourceManager(Graphics* pGraphics) noexcept
	:
	pGraphics(pGraphics)
{
	Heap.Initialize(pGraphics);
}

std::shared_ptr<VertexBuffer> ResourceManager::CreateVertexBuffer(const void* pData, unsigned int Stride, unsigned int DataSize, unsigned int VertexCount, unsigned int Slot) noexcept
{
	using namespace std::string_literals;
	std::string key = typeid(VertexBuffer).name() + "#"s + std::to_string(Stride) + "#"s + std::to_string(DataSize) + "#"s + std::to_string(Slot) + "#"s + "{" + "#"s + std::to_string(((float*)pData)[0]) + "#"s
		+ std::to_string(((float*)pData)[1]) + "#"s + std::to_string(((float*)pData)[3]) + "}";

	const auto i = BaseResources.find(key);
	if (i == BaseResources.end())
	{
		auto res = std::make_shared<VertexBuffer>(pData, Stride, DataSize, VertexCount, Slot);
		BaseResources[key] = res;
		ToInit.push_back(res);
		return res;
	}
	return std::static_pointer_cast<VertexBuffer>(i->second);
}

std::shared_ptr<IndexBuffer> ResourceManager::CreateIndexBuffer(std::vector<unsigned int>* Indecies) noexcept
{
	using namespace std::string_literals;
	std::string key = typeid(IndexBuffer).name() + "#"s + std::to_string(Indecies->size()) + "{" + std::to_string((*Indecies)[0]) + "#"s + std::to_string((*Indecies)[1]) + "#"s + std::to_string((*Indecies)[2]) + "}";
	const auto i = BaseResources.find(key);
	if (i == BaseResources.end())
	{
		auto res = std::make_shared<IndexBuffer>(std::move(Indecies));
		BaseResources[key] = res;
		ToInit.push_back(res);
		return res;
	}
	return std::static_pointer_cast<IndexBuffer>(i->second);
}

std::shared_ptr<RootSignature> ResourceManager::CreateRootSignature(RS_Layout& Lay) noexcept
{
	using namespace std::string_literals;
	const std::string key = typeid(RootSignature).name() + "#"s + Lay.GetCode();
	const auto i = BaseResources.find(key);
	
	if (i == BaseResources.end())
	{
		auto res = std::make_shared<RootSignature>(Lay);
		res->SetKey(key);
		BaseResources[key] = res;
		res->Initialize(pGraphics);
		return res;
	}
	return std::static_pointer_cast<RootSignature>(i->second);
}

std::shared_ptr<PipelineStateObject> ResourceManager::CreatePipelineStateObject(PSO_Layout& pLay, VertexLayout& vLay, std::shared_ptr<RootSignature> pRootSignature) noexcept
{
	using namespace std::string_literals;
	const std::string key = typeid(PipelineStateObject).name() + "#"s + pLay.GetCode() + "#"s + vLay.GetCode();
	const auto i = BaseResources.find(key);
	if (i == BaseResources.end())
	{
		auto res = std::make_shared<PipelineStateObject>(pLay, vLay, pRootSignature);
		res->SetKey(key);
		BaseResources[key] = res;
		ToInit.push_back(res);
		return res;
	}
	return std::static_pointer_cast<PipelineStateObject>(i->second);
}

std::shared_ptr<ConstantBuffer> ResourceManager::CreateConstBuffer(const void* pData, unsigned int DataSize, UINT Index) noexcept
{
	auto res = std::make_shared<ConstantBuffer>(pData, DataSize);
	res->SetHeapIndex(Index);

	// Increment desc count.
	Heap.IncrementDescCountForCbvSrvUav();

	ConstBuffers.push_back(res);

	return res;
}

void ResourceManager::InitializeResources()
{
	if (Heap.IsNeedUpdateHeap0())
	{
		Heap.UpdateHeap0(pGraphics);

		for (auto& rs : HeapResources)
		{
			Heap.SetResource(rs.second);
			rs.second->Initialize(pGraphics);
		}

		for (auto& cb : ConstBuffers)
		{
			Heap.SetResource(cb);
			cb->Initialize(pGraphics);
		}
	}

	if (Heap.IsNeedUpdateHeap1())
	{
		Heap.UpdateHeap1(pGraphics);

		for (auto& s : Samplers)
		{
			Heap.SetSampler(s.second);
			s.second->Initialize(pGraphics);
		}
	}

	for (auto& res : ToInit)
		res->Initialize(pGraphics);

	ToInit.clear();
}

ResourceManager::~ResourceManager()
{
}

std::shared_ptr<Texture2D> ResourceManager::CreateTexture2D(const std::string& Path, UINT Index, bool OnlyPixelShader)
{
	const auto i = HeapResources.find(Path);
	if (i == HeapResources.end())
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
		auto res = std::make_shared<Texture2D>(std::move(img), &textureDesc, OnlyPixelShader);
		res->SetHeapIndex(Index);
		HeapResources[Path] = res;

		// Increment desc count.
		Heap.IncrementDescCountForCbvSrvUav();

		return res;
	}
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
		Heap.IncrementDescCountForSamplers();

		Samplers[key] = res;
		return res;
	}

	return std::static_pointer_cast<Sampler>(i->second);
}

void ResourceManager::Update()
{
}

void ResourceManager::BindHeap()
{
	Heap.Bind(pGraphics);
}

