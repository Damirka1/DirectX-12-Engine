#include "..\..\Headers\ResourceManager.h"
#include "..\..\Headers\Graphics\Resources\Bindable.h"
#include "..\..\Headers\Graphics\Resources\BindablesHeader.h"

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

std::shared_ptr<ConstantBuffer> ResourceManager::CreateConstBuffer(Graphics* pGraphics, void* pData, unsigned int DataSize, unsigned int ParameterIndex)
{
	using namespace std::string_literals;
	const std::string key = typeid(ConstantBuffer).name() + '{' + std::to_string(DataSize) + ':' + std::to_string(static_cast<float*>(pData)[0]) + ':' + std::to_string(ParameterIndex) + '}';
	const auto i = Bindables.find(key);
	if (i == Bindables.end())
	{
		auto bind = std::make_shared<ConstantBuffer>(pGraphics, pData, DataSize, ParameterIndex);
		Bindables[key] = bind;
		return bind;
	}
	else
	{
		return std::static_pointer_cast<ConstantBuffer>(i->second);
	}
}

std::shared_ptr<Texture2D> ResourceManager::CreateTexture2D(Graphics* pGraphics, std::string_view Path, void* pData, unsigned int DataSize, unsigned int BitsPerPixel, void* pDesc, unsigned int ParameterIndex)
{
	auto* pd = static_cast<D3D12_RESOURCE_DESC*>(pDesc);
	using namespace std::string_literals;
	const std::string key = typeid(Texture2D).name() + ':' + std::string(Path) + ':' + std::to_string(pd->Dimension) + ':' + std::to_string(pd->SampleDesc.Count) + ':' + std::to_string(BitsPerPixel);
	const auto i = Bindables.find(key);
	if (i == Bindables.end())
	{
		auto bind = std::make_shared<Texture2D>(pGraphics, pData, DataSize, BitsPerPixel, static_cast<D3D12_RESOURCE_DESC*>(pDesc), ParameterIndex);
		Bindables[key] = bind;
		return bind;
	}
	else
	{
		return std::static_pointer_cast<Texture2D>(i->second);
	}
}
