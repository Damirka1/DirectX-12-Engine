#include "..\..\Headers\ResourceManager.h"
#include "..\..\Headers\Graphics\Resources\Bindable.h"
#include "..\..\Headers\Graphics\Resources\BindablesHeader.h"
#include "..\..\Headers\DirectXTex.h"
#include "..\..\Headers\Utility.h"

std::shared_ptr<VertexBuffer> ResourceManager::CreateVertexBuffer(Graphics* pGraphics, void* pData, unsigned int Stride, unsigned int DataSize, VertexLayout Lay, unsigned int Slot)
{
	using namespace std::string_literals;
	const std::string key = typeid(VertexBuffer).name() + "#"s + std::to_string(Stride) + "#"s + std::to_string(DataSize) + "#"s + std::to_string(Slot) + "{" + Lay.GetCode() + "}";
	const auto i = Bindables.find(key);
	if (i == Bindables.end())
	{
		auto bind = std::make_shared<VertexBuffer>(pGraphics, pData, Stride, DataSize, Slot);
		Bindables[key] = bind;
		return bind;
	}
	else
	{
		return std::static_pointer_cast<VertexBuffer>(i->second);
	}
}

std::shared_ptr<IndexBuffer> ResourceManager::CreateIndexBuffer(Graphics* pGraphics, std::vector<unsigned int> Indecies)
{
	using namespace std::string_literals;
	const std::string key = typeid(IndexBuffer).name() + "#"s + std::to_string(Indecies.size()) + "{" + std::to_string(Indecies[0]) + "}";
	const auto i = Bindables.find(key);
	if (i == Bindables.end())
	{
		auto bind = std::make_shared<IndexBuffer>(pGraphics, std::move(Indecies));
		Bindables[key] = bind;
		return bind;
	}
	else
	{
		return std::static_pointer_cast<IndexBuffer>(i->second);
	}
}

std::shared_ptr<RootSignature> ResourceManager::CreateNullRootSignature(Graphics* pGraphics)
{
	using namespace std::string_literals;
	const std::string key = typeid(RootSignature).name() + "#"s + "NULL";
	const auto i = Bindables.find(key);
	if (i == Bindables.end())
	{
		auto bind = std::make_shared<RootSignature>(pGraphics, "NULL");
		Bindables[key] = bind;
		return bind;
	}
	else
	{
		return std::static_pointer_cast<RootSignature>(i->second);
	}
}

std::shared_ptr<RootSignature> ResourceManager::CreateRootSignature(Graphics* pGraphics, RS_Layout& Lay)
{
	std::string code = Lay.GetCode();
	using namespace std::string_literals;
	const std::string key = typeid(RootSignature).name() + "#"s + code;
	const auto i = Bindables.find(key);
	if (i == Bindables.end())
	{
		auto bind = std::make_shared<RootSignature>(pGraphics, Lay, code);
		Bindables[key] = bind;
		return bind;
	}
	else
	{
		return std::static_pointer_cast<RootSignature>(i->second);
	}
}

std::shared_ptr<PipelineStateObject> ResourceManager::CreatePSO(Graphics* pGraphics, PSO_Layout& pLay, VertexLayout* vLay, RootSignature* RS)
{
	using namespace std::string_literals;
	const std::string key = typeid(PipelineStateObject).name() + "#"s + pLay.GetCode() + "#"s + vLay->GetCode() + RS->GetCode();
	const auto i = Bindables.find(key);
	if (i == Bindables.end())
	{
		auto bind = std::make_shared<PipelineStateObject>(pGraphics, pLay, vLay, RS);
		Bindables[key] = bind;
		return bind;
	}
	else
	{
		return std::static_pointer_cast<PipelineStateObject>(i->second);
	}
}

std::shared_ptr<ConstantBuffer> ResourceManager::CreateConstBuffer(Graphics* pGraphics, void* pData, unsigned int DataSize, CD3DX12_CPU_DESCRIPTOR_HANDLE Handle)
{
	return std::make_shared<ConstantBuffer>(pGraphics, pData, DataSize, Handle);
}

std::shared_ptr<Texture2D> ResourceManager::CreateTexture2D(Graphics* pGraphics, std::string Path, CD3DX12_CPU_DESCRIPTOR_HANDLE Handle)
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
		size_t test1 = img->GetPixelsSize();

		// Describe and create a Texture2D.
		D3D12_RESOURCE_DESC textureDesc = {};
		textureDesc.MipLevels = 1;
		textureDesc.Format = MetaData.format; //pGraphics->GetFormat();
		textureDesc.Width = static_cast<UINT>(MetaData.width);
		textureDesc.Height = static_cast<UINT>(MetaData.height);
		textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		textureDesc.DepthOrArraySize = 1;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		// Create texture pointer.
		auto bind = std::make_shared<Texture2D>(pGraphics, static_cast<void*>(img->GetPixels()), static_cast<UINT>(img->GetPixelsSize()), &textureDesc, &Handle);
		Bindables[Path] = bind;
		return bind;
	}
	else
	{
		return std::static_pointer_cast<Texture2D>(i->second);
	}
}

std::shared_ptr<Sampler> ResourceManager::CreateDefaultSampler(Graphics* pGraphics, CD3DX12_CPU_DESCRIPTOR_HANDLE Handle)
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
	
	return std::make_shared<Sampler>(pGraphics, sampler, &Handle);
}

