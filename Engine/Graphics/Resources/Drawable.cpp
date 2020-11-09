#include "..\..\Headers\Graphics\Resources\Drawable.h"
#include "..\..\Headers\Graphics\Resources\BindablesHeader.h"
#include "..\..\Headers\ResourceManager.h"

void Drawable::AddBindable(std::shared_ptr<Bindable> Bindable)
{
	Bindables.push_back(std::move(Bindable));
}

void Drawable::SetVertexbuffer(std::shared_ptr<VertexBuffer> pVB)
{
	pVertexBuffer = std::move(pVB);
}

void Drawable::SetIndexBuffer(std::shared_ptr<IndexBuffer> pIB)
{
	pIndexBuffer = std::move(pIB);
}

std::shared_ptr<ConstantBuffer> Drawable::CreateConstBuffer(Graphics* pGraphics, void* pData, unsigned int DataSize, unsigned int RootParameterIndex)
{
	CheckArray();

	std::shared_ptr<ConstantBuffer> buffer = std::make_shared<ConstantBuffer>(pGraphics, pData, DataSize, RootParameterIndex);
	std::static_pointer_cast<HeapDescriptorArray>(Bindables[HeapArrayIndex])->
		AddDescriptor(buffer->GetHeap().first, RootParameterIndex);
	return buffer;
}

std::shared_ptr<Texture2D> Drawable::CreateTexture2D(Graphics* pGraphics, std::string Path, unsigned int RootParameterIndex)
{
	return nullptr;
}

void Drawable::CheckArray()
{
	if (HeapArrayIndex == -1)
	{
		AddBindable(std::make_shared<HeapDescriptorArray>());
		HeapArrayIndex = static_cast<int>(Bindables.size()) - 1;
	}
}


void Drawable::Bind(Graphics* pGraphics)
{
	for (std::shared_ptr<Bindable>& e : Bindables)
	{
		e->Bind(pGraphics);
	}
}

void Drawable::Draw(Graphics* pGraphics)
{
	pVertexBuffer->Bind(pGraphics);
	pGraphics->GetCommandList()->DrawInstanced(pVertexBuffer->VertexCount, 1, 0, 0);
}

void Drawable::DrawIndexed(Graphics* pGraphics)
{
	pVertexBuffer->Bind(pGraphics);
	pIndexBuffer->Bind(pGraphics);
	pGraphics->GetCommandList()->DrawIndexedInstanced(pIndexBuffer->Indecies, 1, 0, 0, 0);
}


