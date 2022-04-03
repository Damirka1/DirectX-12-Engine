#include "../Headers/FrameCommander.h"
#include "../Headers/Window.h"
#include "../Headers/Scene/Scene.h"

FrameCommander::FrameCommander(Window* pWindow, ResourceManager* pRM) noexcept
	:
	pGraphics(pWindow->GetGraphics()),
	pResourceManager(pRM)
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
		pResourceManager->InitializeResources(&*pScene);
	else
		throw std::exception("Set scene to frame commander befor preparetion");
}

void FrameCommander::Render()
{
	pGraphics->Setup(BackgroundColor.x, BackgroundColor.y, BackgroundColor.z);

	// Bind global heap.
	pResourceManager->Heap.Bind(pGraphics);

	//// Bind pipelinestate object.
	//for (auto& PSO : pScene->DrawablesMap)
	//{
	//	PSO.second.pPipeLineStateObject->Bind(pGraphics);

	//	// Bind rootsignatures.
	//	for (auto& RS : PSO.second.RootSignatures)
	//	{
	//		RS.second.pRootSignature->Bind(pGraphics);

	//		// And finaly render objects.
	//		for (auto& obj : RS.second.DrawArrayIndexed)
	//			obj.second->DrawIndexed(pGraphics);
	//	}
	//}

	for (auto& m : pScene->Models)
		m->Draw(pGraphics);

	pGraphics->Execute();
}

FrameCommander::~FrameCommander()
{
}

FrameCommanderHWND::FrameCommanderHWND(Window* pWindow, ResourceManager* pRM) noexcept
	:
	FrameCommander(pWindow, pRM)
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
