#include "..\..\Headers\Graphics\Resources\DrawableArray.h"
#include "..\..\Headers\Graphics\Resources\Drawable.h"

void DrawableArray::AddDrawable(Drawable* pDrawable)
{
	pDrawables.insert(pDrawable);
}

void DrawableArray::RemoveDrawable(Drawable* pDrawable)
{
	pDrawables.erase(pDrawable);
}

void DrawableArray::DrawIndexed(Graphics* pGraphics)
{
	pVertexBuffer->Bind(pGraphics);
	pIndexBuffer->Bind(pGraphics);
	ID3D12GraphicsCommandList6* pCommandList = pGraphics->GetCommandList();

	for (Drawable* obj : pDrawables)
	{
		obj->Bind(pGraphics);
		pCommandList->DrawIndexedInstanced(pIndexBuffer->IndeciesCount, 1, 0, 0, 0);
	}
}

void DrawableArray::SetVertexAndIndexBuffers(std::shared_ptr<VertexBuffer> pVB, std::shared_ptr<IndexBuffer> pIB)
{
	pVertexBuffer = pVB;
	pIndexBuffer = pIB;
}