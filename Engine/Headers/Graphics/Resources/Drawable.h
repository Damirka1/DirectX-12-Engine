//#pragma once
//#ifndef DRAWABLE_HEADER
//#define DRAWABLE_HEADER
//#include <vector>
//#include <memory>
//#include "Bindable.h"
//#include "..\..\ResourceManager.h"
//#include "..\..\Events\EventListener.h"
//
//class ResourceManager;
//class FrameCommander;
//class DrawableArray;
//class UI_Element;
//class Camera;
//class Scene;
//
//class Drawable
//{
//	friend ResourceManager;
//	friend FrameCommander;
//	friend DrawableArray;
//	friend UI_Element;
//	friend ScriptManager;
//	friend Scene;
//
//protected:
//	Engine_API Drawable() noexcept;
//	Engine_API Drawable(Camera* cam) noexcept;
//	Engine_API void Init(Camera* cam) noexcept;
//	Engine_API void AddResource(std::shared_ptr<Resource> Resource) noexcept;
//
//public:
//	Engine_API DirectX::XMFLOAT3 GetPos();
//	Engine_API void SetPos(DirectX::XMFLOAT3 Pos);
//	Engine_API void Translate(DirectX::XMFLOAT3 T);
//	Engine_API void SetVisibility(bool Visible);
//
//	Engine_API bool IsReady();
//
//	Engine_API void AddEventListener(EventListener* EvListener);
//	Engine_API void RemoveEventListener(EventListener* EvListener);
//	Engine_API const std::unordered_map<LONG_PTR, EventListener*>* const GetEventListeners();
//
//	Engine_API void operator+=(EventListener* EvListener);
//	Engine_API void operator-=(EventListener* EvListener);
//	Engine_API void operator=(EventListener* EvListener);
//
//protected:
//	bool Visible = true;
//	bool Ready = false;
//
//	DirectX::XMMATRIX Transformation;
//	DirectX::XMFLOAT3 Pos;
//
//	struct
//	{
//		const DirectX::XMMATRIX* View;
//		const DirectX::XMMATRIX* Projection;
//	} pCamera;
//
//	std::shared_ptr<DrawableArray> Array;
//private:
//	std::unordered_map<LONG_PTR, EventListener*> EventListeners;
//
//	std::vector<std::shared_ptr<Resource>> Resources;
//};
//
//#endif