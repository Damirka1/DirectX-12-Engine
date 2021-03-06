#pragma once
#ifndef DRAWABLE_HEADER
#define DRAWABLE_HEADER
#include <vector>
#include <memory>
#include "Bindable.h"
#include "..\..\ResourceManager.h"
#include "BindablesHeader.h"
#include "..\..\Events\EventListener.h"

class ResourceManager;
class FrameCommander;
class DrawableArray;
class UI_Element;
class ScriptManager;

class Drawable
{
	friend ResourceManager;
	friend FrameCommander;
	friend DrawableArray;
	friend UI_Element;
	friend ScriptManager;

protected:
	Engine_API Drawable() noexcept;
	Engine_API Drawable(ResourceManager* pRM) noexcept;
	Engine_API void Init(ResourceManager* pRM) noexcept;
	Engine_API void AddBindable(std::shared_ptr<Bindable> Bindable) noexcept;
	Engine_API void Bind(Graphics* pGraphics);
	Engine_API void SetVertexAndIndexBuffers(std::shared_ptr<VertexBuffer> pVB, std::shared_ptr<IndexBuffer> pIB) noexcept;

public:
	Engine_API DirectX::XMFLOAT3 GetPos();
	Engine_API void SetPos(DirectX::XMFLOAT3 Pos);
	Engine_API void Translate(DirectX::XMFLOAT3 T);
	Engine_API void SetVisibility(bool Visible);

	Engine_API void AddEventListener(EventListener* EvListener);
	Engine_API void RemoveEventListener(EventListener* EvListener);
	Engine_API const std::unordered_map<LONG_PTR, EventListener*>* const GetEventListeners();

	Engine_API void operator+=(EventListener* EvListener);
	Engine_API void operator-=(EventListener* EvListener);
	Engine_API void operator=(EventListener* EvListener);
	Engine_API virtual void Update();

protected:
	bool UI_element = false;
	bool Visible = true;

	DirectX::XMMATRIX Transformation;
	DirectX::XMFLOAT3 Pos;

	struct
	{
		const DirectX::XMMATRIX* View;
		const DirectX::XMMATRIX* Projection;
	} pCamera;

private:
	std::vector<std::shared_ptr<Bindable>> Bindables;

	std::shared_ptr<VertexBuffer> pVertexBuffer;
	std::shared_ptr<IndexBuffer> pIndexBuffer;

	std::vector<std::shared_ptr<Bindable>> InitializeHeap_list;
	std::vector<std::shared_ptr<Bindable>> InitializeCommon_list;
	// Index of Descriptor heap.
	char DescHeapIndex = -1;
	
	std::string PSO_Key, RS_Key;

	std::unordered_map<LONG_PTR, EventListener*> EventListeners;
};

#endif