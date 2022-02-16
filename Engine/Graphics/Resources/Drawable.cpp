#include "../../Headers/Graphics/Resources/Drawable.h"
#include "../../Headers/Graphics/Resources/BindablesHeader.h"
#include "../../Headers/Input/Camera.h"

Drawable::Drawable() noexcept
{
}

Drawable::Drawable(Camera* cam) noexcept
{
	Init(cam);
}

void Drawable::Init(Camera* cam) noexcept
{
	pCamera.Projection = &cam->GetProjection();
	pCamera.View = &cam->GetView();
}

Engine_API void Drawable::AddResource(std::shared_ptr<Resource> Resource) noexcept
{
	Resources.push_back(std::move(Resource));
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
		EventListeners[LONG_PTR(EvListener)] = EvListener;
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

