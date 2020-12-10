#include "..\..\Headers\Graphics\Resources\Drawable.h"
#include "..\..\Headers\Graphics\Resources\BindablesHeader.h"
#include "..\..\Headers\ResourceManager.h"


void Drawable::AddBindable(std::shared_ptr<Bindable> Bindable)
{
	Bindables.push_back(std::move(Bindable));
}

void Drawable::Bind(Graphics* pGraphics)
{
	for (std::shared_ptr<Bindable>& e : Bindables)
	{
		e->Bind(pGraphics);
	}
}

void Drawable::SetVertexAndIndexBuffers(std::shared_ptr<VertexBuffer> pVB, std::shared_ptr<IndexBuffer> pIB)
{
	pVertexBuffer = std::move(pVB);
	pIndexBuffer = std::move(pIB);
}



