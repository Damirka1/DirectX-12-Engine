#pragma once
#ifndef FRAME_COMMANDER_HEADER
#define FRAME_COMMANDER_HEADER
#include "Graphics.h"
#include "ResourceManager.h"

class FrameCommander
{
public:
	FrameCommander() = delete;
	FrameCommander(FrameCommander&) = delete;

	Engine_API FrameCommander(Window* pWindow, ResourceManager* pRM) noexcept;
	Engine_API void SetBackgroundColor(float r, float g, float b) noexcept;
	Engine_API void ChangeBackgroundColor(float dr, float dg, float db) noexcept;
	Engine_API void SetScene(std::shared_ptr<Scene> pScene);

	Engine_API void SetupInit();
	Engine_API void InitializeResources();
	
	Engine_API void Render();
	Engine_API ~FrameCommander();

private:
	Graphics* pGraphics;
	ResourceManager* pResourceManager;
	
	std::shared_ptr<Scene> pScene;
	DirectX::XMFLOAT3 BackgroundColor = { 0.0f, 0.0f, 0.0f };
};


#include "MessageHandler.h"

class FrameCommanderHWND : public FrameCommander, public MessageHandler
{
	friend class Window;
public:
	FrameCommanderHWND() = delete;
	FrameCommanderHWND(FrameCommanderHWND&) = delete;

	Engine_API FrameCommanderHWND(Window* pWindow, ResourceManager* pRM) noexcept;
	Engine_API ~FrameCommanderHWND() override = default;

private:
	void CALLBACK HandleMsg(HWND& hWnd, UINT& msg, WPARAM& wParam, LPARAM& lParam) noexcept override;
};

#endif 
