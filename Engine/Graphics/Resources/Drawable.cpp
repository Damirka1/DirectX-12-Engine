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
	pGraphics->GetCommandList()->DrawIndexedInstanced(pIndexBuffer->IndeciesCount, 1, 0, 0, 0);
}


