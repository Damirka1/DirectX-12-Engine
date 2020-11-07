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

std::shared_ptr<ConstantBuffer> Drawable::AddConstBuffer(Graphics* pGraphics, ResourceManager* pManager, void* pData, unsigned int DataSize, unsigned int RootParameterIndex)
{
	if (HeapArrayIndex == -1)
	{
		AddBindable(std::make_shared<HeapDescriptorArray>());
		HeapArrayIndex = static_cast<int>(Bindables.size()) - 1;
	}
	std::shared_ptr<ConstantBuffer> buffer = pManager->CreateConstBuffer(pGraphics, pData, DataSize, RootParameterIndex);
	std::static_pointer_cast<HeapDescriptorArray>(Bindables[HeapArrayIndex])->
		AddDescriptor(buffer->GetHeap().first, RootParameterIndex);
	return buffer;
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
	pGraphics->GetCommandList()->DrawInstanced(pVertexBuffer->GetVertexCount(), 1, 0, 0);
}

void Drawable::DrawIndexed(Graphics* pGraphics)
{
	pVertexBuffer->Bind(pGraphics);
	pIndexBuffer->Bind(pGraphics);
	pGraphics->GetCommandList()->DrawIndexedInstanced(pIndexBuffer->GetIndeciesCount(), 1, 0, 0, 0);
}
