#include "../Headers/FrameCommander.h"
#include "../Headers/Window.h"
#include "../Headers/Scene/Scene.h"
#include "../Headers/Timer.h"

FrameCommander::FrameCommander(Window* pWindow, Timer* pTimer) noexcept
	:
	pGraphics(pWindow->GetGraphics()),
	pTimer(pTimer)
{
}

void FrameCommander::SetBackgroundColor(float r, float g, float b) noexcept
{
	BackgroundColor.x = r;
	BackgroundColor.y = g;
	BackgroundColor.z = b;
}

void FrameCommander::ChangeBackgroundColor(float dr, float dg, float db) noexcept
{
	BackgroundColor.x += dr;
	BackgroundColor.y += dg;
	BackgroundColor.z += db;
}

void FrameCommander::SetScene(std::shared_ptr<Scene> pScene)
{
	if (pScene)
		this->pScene = pScene;
	else
		throw std::exception("Scene was nullptr");
}

void FrameCommander::SetupInit()
{
	pGraphics->SetupInit();
}

void FrameCommander::InitializeResources()
{
	if (pScene)
		pScene->InitializeResources();
	else
		throw std::exception("Set scene to frame commander befor preparetion");
}

void FrameCommander::Update()
{
	pScene->Update();
	//pResourceManager->Update();
}

void FrameCommander::Render()
{
	float dt = pTimer->Mark();
	pGraphics->Setup(BackgroundColor.x, BackgroundColor.y, BackgroundColor.z);

	auto* res = &pScene->SceneResources;

	res->pColorPass->Bind();
	res->pColorPass->Execute();

	res->pTexturePass->Bind();
	res->pTexturePass->Execute();

	res->pRTXPass->Execute();

	res->pPhysx->Simulate(dt);

	pGraphics->Execute();
}

FrameCommander::~FrameCommander()
{
}

FrameCommanderHWND::FrameCommanderHWND(Window* pWindow, Timer* pTimer) noexcept
	:
	FrameCommander(pWindow, pTimer)
{
	pWindow->DisableVSync();
	pWindow->SleepTime = 1;
}

void FrameCommanderHWND::HandleMsg(HWND& hWnd, UINT& msg, WPARAM& wParam, LPARAM& lParam) noexcept
{
	Window* const pWindow = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

	switch (msg)
	{
	case WM_PAINT:
		Render();
		pWindow->UpdateInThisFrame = false;
		return;
	default:
		return;
	}
}
