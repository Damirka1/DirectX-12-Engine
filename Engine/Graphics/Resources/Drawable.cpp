#include "..\..\Headers\Graphics\Resources\Drawable.h"
#include "..\..\Headers\Graphics\Resources\BindablesHeader.h"


Drawable::Drawable() noexcept
{
}

Drawable::Drawable(ResourceManager* pRM) noexcept
{
	Init(pRM);
}

void Drawable::Init(ResourceManager* pRM) noexcept
{
	if (UI_element)
	{
		pCamera.Projection = pRM->GetProjectionForUI();
		pCamera.View = pRM->GetViewForUI();
	}
	else
	{
		pCamera.Projection = pRM->GetPerspectiveProjection();
		pCamera.View = pRM->GetView();
	}
}

void Drawable::AddBindable(std::shared_ptr<Bindable> Bindable) noexcept
{
	Bindables.push_back(std::move(Bindable));
}

Engine_API void Drawable::AddResource(std::shared_ptr<Resource> Resource) noexcept
{
	Resources.push_back(std::move(Resource));
}

void Drawable::Bind(Graphics* pGraphics)
{
	for (std::shared_ptr<Bindable>& e : Bindables)
		e->Bind(pGraphics);
}

void Drawable::SetVertexAndIndexBuffers(std::shared_ptr<VertexBuffer> pVB, std::shared_ptr<IndexBuffer> pIB) noexcept
{
	pVertexBuffer = std::move(pVB);
	pIndexBuffer = std::move(pIB);
}

void Drawable::SetVisibility(bool Visible)
{
	this->Visible = Visible;
}

DirectX::XMFLOAT3 Drawable::GetPos()
{
	return Pos;
}

void Drawable::SetPos(DirectX::XMFLOAT3 Pos)
{
	this->Pos = Pos;
}

void Drawable::Translate(DirectX::XMFLOAT3 T)
{
	Pos.x += T.x;
	Pos.y += T.y;
	Pos.z += T.z;
}

void Drawable::AddEventListener(EventListener* EvListener)
{
	const auto i = EventListeners.find(LONG_PTR(EvListener));
	if (i == EventListeners.end())
	{
		EventListeners[LONG_PTR(EvListener)] = EvListener;
	}
}

void Drawable::RemoveEventListener(EventListener* EvListener)
{
	EventListeners.erase(LONG_PTR(EvListener));
}

const std::unordered_map<LONG_PTR, EventListener*>* const Drawable::GetEventListeners()
{
	return &EventListeners;
}

void Drawable::operator+=(EventListener* EvListener)
{
	AddEventListener(EvListener);
}

void Drawable::operator-=(EventListener* EvListener)
{
	RemoveEventListener(EvListener);
}

void Drawable::operator=(EventListener* EvListener)
{
	EventListeners.clear();
	EventListeners[LONG_PTR(EvListener)] = (EvListener);
}

//void Drawable::Update()
//{
//}

