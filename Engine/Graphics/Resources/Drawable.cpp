#include "..\..\Headers\Graphics\Resources\Drawable.h"
#include "..\..\Headers\Graphics\Resources\BindablesHeader.h"


Drawable::Drawable(ResourceManager* pRM) noexcept
{
	Init(pRM);
}

void Drawable::Init(ResourceManager* pRM) noexcept
{
	if (UI_element)
		Transformation.pProjection = pRM->GetProjectionForUI();
	else
	{
		Transformation.pProjection = pRM->GetPerspectiveProjection();
		Transformation.pView = pRM->GetView();
	}
}

void Drawable::AddBindable(std::shared_ptr<Bindable> Bindable) noexcept
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

void Drawable::Update() const
{
}

void Drawable::SetVertexAndIndexBuffers(std::shared_ptr<VertexBuffer> pVB, std::shared_ptr<IndexBuffer> pIB) noexcept
{
	pVertexBuffer = std::move(pVB);
	pIndexBuffer = std::move(pIB);
}



