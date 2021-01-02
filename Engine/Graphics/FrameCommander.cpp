#include "..\Headers\FrameCommander.h"
#include "..\Headers\Window.h"

FrameCommander::FrameCommander(Window* pWindow, ResourceManager* pRM) noexcept
	:
	pGraphics(pWindow->GetGraphics()),
	pRM(pRM)
{
}

void FrameCommander::SetBackgroundColor(float r, float g, float b) noexcept
{
	bg[0] = r;
	bg[1] = g;
	bg[2] = b;
}

void FrameCommander::ChangeBackgroundColor(float dr, float dg, float db) noexcept
{
	bg[0] += dr;
	bg[1] += dg;
	bg[2] += db;
}

void FrameCommander::Render()
{
	pGraphics->Setup(bg[0], bg[1], bg[2]);

	// Bind global heap.
	pRM->Heap.Bind(pGraphics);

	auto Render = [&](std::unordered_map<std::string, ResourceManager::PipeLineResources>& Resources)
	{
		// Bind pipelinestate object.
		for (auto& PSO : Resources)
		{
			PSO.second.pPipeLineStateObject->Bind(pGraphics);

			// Bind rootsignatures.
			for (auto& RS : PSO.second.RootSignatures)
			{
				RS.second.pRootSignature->Bind(pGraphics);

				// And finaly render objects.
				for (auto& obj : RS.second.DrawIndexed)
				{
					obj.second.DrawIndexed(pGraphics);
				}
			}
		}
	};

	Render(pRM->Resources);

	Render(pRM->UI_Resources);

	pGraphics->Execute();
}

FrameCommanderHWND::FrameCommanderHWND(Window* pWindow, ResourceManager* pRM) noexcept
	:
	FrameCommander(pWindow, pRM)
{
	pWindow->DisableVSync();
	pWindow->SleepTime = 10;
}

void FrameCommanderHWND::HandleMsg(HWND& hWnd, UINT& msg, WPARAM& wParam, LPARAM& lParam) noexcept
{
	Window* const pWindow = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	switch (msg)
	{
	case WM_SIZE:
	{
		if (wParam == SIZE_MINIMIZED)
			pWindow->Visible = false;
		else if (wParam == SIZE_RESTORED)
			pWindow->Visible = true;
		return;
	}
	case WM_PAINT:
		if(pWindow->Visible)
			Render();
		return;
	default:
		return;
	}
}
